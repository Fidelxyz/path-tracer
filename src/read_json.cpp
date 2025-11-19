#include "read_json.h"

#include <Eigen/Geometry>
#include <cassert>
#include <fstream>
#include <iostream>
#include <json.hpp>

#include "DirectionalLight.h"
#include "Light.h"
#include "Material.h"
#include "Object.h"
#include "Plane.h"
#include "PointLight.h"
#include "Sphere.h"
#include "Triangle.h"
#include "TriangleSoup.h"
#include "dirname.h"
#include "readSTL.h"

bool read_json(const std::string& filename, Camera& camera,
               std::vector<std::shared_ptr<Object> >& objects,
               std::vector<std::shared_ptr<Light> >& lights) {
    // Heavily borrowing from
    // https://github.com/yig/graphics101-raycasting/blob/master/parser.cpp
    using json = nlohmann::json;

    std::ifstream infile(filename);
    if (!infile) return false;
    json j;
    infile >> j;

    // parse a vector
    auto parse_Vector3f = [](const json& j) -> Eigen::Vector3f {
        return Eigen::Vector3f(j[0], j[1], j[2]);
    };
    // parse camera
    auto parse_camera = [&parse_Vector3f](const json& j, Camera& camera) {
        assert(j["type"] == "perspective" &&
               "Only handling perspective cameras");
        camera.d = j["focal_length"].get<double>();
        camera.e = parse_Vector3f(j["eye"]);
        camera.v = parse_Vector3f(j["up"]).normalized();
        camera.w = -parse_Vector3f(j["look"]).normalized();
        camera.u = camera.v.cross(camera.w);
        camera.height = j["height"].get<double>();
        camera.width = j["width"].get<double>();
    };
    parse_camera(j["camera"], camera);

    // Parse materials
    std::unordered_map<std::string, std::shared_ptr<Material> > materials;
    auto parse_materials =
        [&parse_Vector3f](
            const json& j,
            std::unordered_map<std::string, std::shared_ptr<Material> >&
                materials) {
            materials.clear();
            for (const json& jmat : j) {
                std::string name = jmat["name"];
                std::shared_ptr<Material> material(new Material());
                material->ka = parse_Vector3f(jmat["ka"]);
                material->kd = parse_Vector3f(jmat["kd"]);
                material->ks = parse_Vector3f(jmat["ks"]);
                material->km = parse_Vector3f(jmat["km"]);
                material->phong_exponent = jmat["phong_exponent"];
                materials[name] = material;
            }
        };
    parse_materials(j["materials"], materials);

    auto parse_lights = [&parse_Vector3f](
                            const json& j,
                            std::vector<std::shared_ptr<Light> >& lights) {
        lights.clear();
        for (const json& jlight : j) {
            if (jlight["type"] == "directional") {
                std::shared_ptr<DirectionalLight> light(new DirectionalLight());
                light->d = parse_Vector3f(jlight["direction"]).normalized();
                light->I = parse_Vector3f(jlight["color"]);
                lights.push_back(light);
            } else if (jlight["type"] == "point") {
                std::shared_ptr<PointLight> light(new PointLight());
                light->p = parse_Vector3f(jlight["position"]);
                light->I = parse_Vector3f(jlight["color"]);
                lights.push_back(light);
            }
        }
    };
    parse_lights(j["lights"], lights);

    auto parse_objects = [&parse_Vector3f, &filename, &materials](
                             const json& j,
                             std::vector<std::shared_ptr<Object> >& objects) {
        objects.clear();
        for (const json& jobj : j) {
            if (jobj["type"] == "sphere") {
                std::shared_ptr<Sphere> sphere(new Sphere());
                sphere->center = parse_Vector3f(jobj["center"]);
                sphere->radius = jobj["radius"].get<double>();
                objects.push_back(sphere);
            } else if (jobj["type"] == "plane") {
                std::shared_ptr<Plane> plane(new Plane());
                plane->point = parse_Vector3f(jobj["point"]);
                plane->normal = parse_Vector3f(jobj["normal"]).normalized();
                objects.push_back(plane);
            } else if (jobj["type"] == "triangle") {
                std::shared_ptr<Triangle> tri(new Triangle());
                tri->corners =
                    std::make_tuple(parse_Vector3f(jobj["corners"][0]),
                                    parse_Vector3f(jobj["corners"][1]),
                                    parse_Vector3f(jobj["corners"][2]));
                objects.push_back(tri);
            } else if (jobj["type"] == "soup") {
                std::vector<std::vector<double> > V;
                std::vector<std::vector<double> > F;
                std::vector<std::vector<int> > N;
                {
#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR std::string("\\")
#else
#define PATH_SEPARATOR std::string("/")
#endif
                    const std::string stl_path = jobj["stl"];
                    igl::readSTL(
                        igl::dirname(filename) + PATH_SEPARATOR + stl_path, V,
                        F, N);
                }
                std::shared_ptr<TriangleSoup> soup(new TriangleSoup());
                for (size_t f = 0; f < F.size(); f++) {
                    std::shared_ptr<Triangle> tri(new Triangle());
                    tri->corners = std::make_tuple(
                        Eigen::Vector3f(V[F[f][0]][0], V[F[f][0]][1],
                                        V[F[f][0]][2]),
                        Eigen::Vector3f(V[F[f][1]][0], V[F[f][1]][1],
                                        V[F[f][1]][2]),
                        Eigen::Vector3f(V[F[f][2]][0], V[F[f][2]][1],
                                        V[F[f][2]][2]));
                    soup->triangles.push_back(tri);
                }
                objects.push_back(soup);
            }
            // objects.back()->material = default_material;
            if (jobj.count("material")) {
                if (materials.count(jobj["material"])) {
                    objects.back()->material = materials[jobj["material"]];
                }
            }
        }
    };
    parse_objects(j["objects"], objects);

    return true;
}
