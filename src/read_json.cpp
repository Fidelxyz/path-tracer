#include "read_json.h"

#include <dirname.h>
#include <readSTL.h>

#include <Eigen/Geometry>
#include <cassert>
#include <fstream>
#include <json.hpp>

#include "lights/DirectionalLight.h"
#include "lights/PointLight.h"
#include "objects/Sphere.h"
#include "objects/Triangle.h"

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR std::string("\\")
#else
#define PATH_SEPARATOR std::string("/")
#endif

Scene read_json(const std::string& filename) {
    // Heavily borrowing from
    // https://github.com/yig/graphics101-raycasting/blob/master/parser.cpp
    using json = nlohmann::json;

    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    json j;
    infile >> j;

    // parse a vector
    const auto parse_Vector3f = [](const json& j) -> Eigen::Vector3f {
        return Eigen::Vector3f(j[0], j[1], j[2]);
    };

    // parse camera
    const auto parse_camera = [&parse_Vector3f](const json& j) -> Camera {
        assert(j["type"] == "perspective" &&
               "Only handling perspective cameras");
        const float d = j["focal_length"];
        const Eigen::Vector3f e = parse_Vector3f(j["eye"]);
        const Eigen::Vector3f v = parse_Vector3f(j["up"]).normalized();
        const Eigen::Vector3f w = -parse_Vector3f(j["look"]).normalized();
        const Eigen::Vector3f u = v.cross(w);
        const float height = j["height"];
        const float width = j["width"];
        return Camera(e, u, v, w, d, width, height);
    };

    // Parse materials
    const auto parse_materials = [&parse_Vector3f](const json& j)
        -> std::unordered_map<std::string, std::shared_ptr<Material>> {
        std::unordered_map<std::string, std::shared_ptr<Material>> materials;
        materials.reserve(j.size());
        for (const json& jmat : j) {
            const std::string name = jmat["name"];
            materials[name] = std::make_shared<Material>(
                parse_Vector3f(jmat["ka"]), parse_Vector3f(jmat["kd"]),
                parse_Vector3f(jmat["ks"]), parse_Vector3f(jmat["km"]),
                jmat["phong_exponent"]);
        }
        return materials;
    };
    const auto materials = parse_materials(j["materials"]);

    const auto parse_lights = [&parse_Vector3f](const json& j) -> Lights {
        std::vector<std::unique_ptr<Light>> lights;
        lights.reserve(j.size());
        for (const json& jlight : j) {
            if (jlight["type"] == "directional") {
                lights.push_back(std::make_unique<DirectionalLight>(
                    parse_Vector3f(jlight["color"]),
                    parse_Vector3f(jlight["direction"]).normalized()));
            } else if (jlight["type"] == "point") {
                lights.push_back(std::make_unique<PointLight>(
                    parse_Vector3f(jlight["color"]),
                    parse_Vector3f(jlight["position"])));
            }
        }
        lights.shrink_to_fit();
        return lights;
    };

    auto parse_objects =
        [&parse_Vector3f, &filename,
         &materials](const json& j) -> std::vector<std::unique_ptr<Object>> {
        std::vector<std::unique_ptr<Object>> objects;
        objects.reserve(j.size());
        for (const json& jobj : j) {
            const std::shared_ptr<Material> material =
                (jobj.count("material") && materials.count(jobj["material"]))
                    ? materials.at(jobj["material"])
                    : nullptr;

            if (jobj["type"] == "sphere") {
                objects.push_back(std::make_unique<Sphere>(
                    parse_Vector3f(jobj["center"]), jobj["radius"].get<float>(),
                    material));

            } else if (jobj["type"] == "triangle") {
                objects.push_back(std::make_unique<Triangle>(
                    parse_Vector3f(jobj["corners"][0]),
                    parse_Vector3f(jobj["corners"][1]),
                    parse_Vector3f(jobj["corners"][2]), material));

            } else if (jobj["type"] == "soup") {
                std::vector<std::vector<float>> V;
                std::vector<std::vector<float>> F;
                std::vector<std::vector<int>> N;
                igl::readSTL(igl::dirname(filename) + PATH_SEPARATOR +
                                 jobj["stl"].get<std::string>(),
                             V, F, N);

                for (size_t f = 0; f < F.size(); f++) {
                    objects.push_back(std::make_unique<Triangle>(
                        Eigen::Vector3f(V[F[f][0]][0], V[F[f][0]][1],
                                        V[F[f][0]][2]),
                        Eigen::Vector3f(V[F[f][1]][0], V[F[f][1]][1],
                                        V[F[f][1]][2]),
                        Eigen::Vector3f(V[F[f][2]][0], V[F[f][2]][1],
                                        V[F[f][2]][2]),
                        material));
                }
            }
        }
        objects.shrink_to_fit();
        return objects;
    };

    return Scene(parse_camera(j["camera"]), parse_objects(j["objects"]),
                 parse_lights(j["lights"]));
}
