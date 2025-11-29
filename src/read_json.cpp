#include "read_json.h"

#include <igl/readSTL.h>
#include <tiny_obj_loader.h>

#include <Eigen/Geometry>
#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <json.hpp>
#include <memory>

#include "geometry/Sphere.h"
#include "geometry/Triangle.h"
#include "light/DirectionalLight.h"
#include "light/PointLight.h"

using json = nlohmann::json;

namespace {

template <typename T>
static Eigen::Vector3f to_vector3f(const T& x) {
    return Eigen::Vector3f(x[0], x[1], x[2]);
}

static Camera parse_camera(const json& j) {
    if (!j.contains("camera")) {
        std::cerr << "No camera found in JSON file.\n";
        exit(EXIT_FAILURE);
    }

    const json& j_cam = j["camera"];

    assert(j_cam["type"] == "perspective" &&
           "Only handling perspective cameras");

    const Eigen::Vector3f position = to_vector3f(j_cam["position"]);
    const Eigen::Vector3f rotation = to_vector3f(j_cam["rotation"]);
    const float focal_length = j_cam["focal_length"];
    const float height = j_cam["height"];
    const float width = j_cam["width"];
    const unsigned int resolution_x = j_cam["resolution_x"];
    const unsigned int resolution_y = j_cam["resolution_y"];
    const unsigned int samples = j_cam["samples"];
    const float exposure = j_cam.value("exposure", 1.F);
    return {position,     rotation,     focal_length, width,   height,
            resolution_x, resolution_y, samples,      exposure};
}

static std::vector<std::unique_ptr<Light>> parse_lights(const json& j) {
    if (!j.contains("lights")) return {};
    const json& j_lights = j["lights"];

    std::vector<std::unique_ptr<Light>> lights;
    lights.reserve(j_lights.size());
    for (const json& j_light : j_lights) {
        const Eigen::Vector3f intensity =
            j_light["intensity"] * to_vector3f(j_light["color"]);

        if (j_light["type"] == "directional") {
            lights.emplace_back(std::make_unique<DirectionalLight>(
                intensity, to_vector3f(j_light["direction"]).normalized()));

        } else if (j_light["type"] == "point") {
            lights.emplace_back(std::make_unique<PointLight>(
                intensity, to_vector3f(j_light["position"]),
                j_light["radius"]));

        } else {
            std::cout << "Unknown light type: " << j_light["type"] << "\n";
        }
    }
    lights.shrink_to_fit();
    return lights;
}

static std::unordered_map<std::string, std::shared_ptr<Material>>
parse_materials(const json& j) {
    if (!j.contains("materials")) return {};
    const json& j_materials = j["materials"];

    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
    materials.reserve(j_materials.size());
    for (const json& jmat : j_materials) {
        const std::string name = jmat["name"];

        const auto ambient = jmat.contains("ambient")
                                 ? to_vector3f(jmat["ambient"])
                                 : Eigen::Vector3f::Zero();
        const auto diffuse = jmat.contains("diffuse")
                                 ? to_vector3f(jmat["diffuse"])
                                 : Eigen::Vector3f::Ones();
        const auto specular = jmat.contains("specular")
                                  ? to_vector3f(jmat["specular"])
                                  : Eigen::Vector3f::Zero();
        const auto transmittance = jmat.contains("transmittance")
                                       ? to_vector3f(jmat["transmittance"])
                                       : Eigen::Vector3f::Zero();
        const auto emission = jmat.contains("emission")
                                  ? to_vector3f(jmat["emission"])
                                  : Eigen::Vector3f::Zero();
        const float shininess = jmat.value("shininess", 0.F);
        const float ior = jmat.value("ior", 1.F);

        materials[name] =
            std::make_shared<Material>(ambient, diffuse, specular,
                                       transmittance, emission, shininess, ior);
    }
    return materials;
}

static std::vector<std::unique_ptr<Geometry>> read_stl(
    const json& jobj, const std::shared_ptr<Material>& material,
    const std::filesystem::path& base_path) {
    std::ifstream stl_file(base_path / jobj["stl"], std::ios::binary);
    if (!stl_file) {
        std::cerr << "Error opening STL file: "
                  << jobj["stl"].get<std::string>() << '\n';
        exit(EXIT_FAILURE);
    }

    Eigen::MatrixXf V;
    Eigen::MatrixXi F;
    Eigen::MatrixXf N;
    igl::readSTL(stl_file, V, F, N);

    std::vector<std::unique_ptr<Geometry>> objects;
    objects.reserve(F.rows());
    for (const auto& f : F.rowwise()) {
        objects.emplace_back(std::make_unique<Triangle>(
            std::make_tuple(V.row(f[0]), V.row(f[1]), V.row(f[2])), material));
    }

    return objects;
}

static std::vector<std::unique_ptr<Geometry>> read_obj(
    const json& jobj, const std::filesystem::path& base_path) {
    const std::filesystem::path obj_file = base_path / jobj["obj"];

    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = obj_file.parent_path();

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(obj_file, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(EXIT_FAILURE);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& tinyobj_attrib = reader.GetAttrib();
    auto& tinyobj_shapes = reader.GetShapes();
    auto& tinyobj_materials = reader.GetMaterials();

    // Materials
    std::vector<std::shared_ptr<Material>> materials;
    materials.reserve(tinyobj_materials.size());
    for (auto& material : tinyobj_materials) {
        materials.emplace_back(std::make_shared<Material>(
            Material{.ambient = to_vector3f(material.ambient),
                     .diffuse = to_vector3f(material.diffuse),
                     .specular = to_vector3f(material.specular),
                     .transmittance = to_vector3f(material.transmittance),
                     .emission = to_vector3f(material.emission),
                     .shininess = material.shininess,
                     .ior = material.ior,
                     .roughness = material.roughness,
                     .metallic = material.metallic,
                     .sheen = material.sheen,
                     .clearcoat_thickness = material.clearcoat_thickness,
                     .clearcoat_roughness = material.clearcoat_roughness,
                     .anisotropy = material.anisotropy,
                     .anisotropy_rotation = material.anisotropy_rotation,
                     .emissive = to_vector3f(material.emission).any()}));
    }

    std::vector<std::unique_ptr<Geometry>> objects;

    for (const auto& shape : tinyobj_shapes) {
        objects.reserve(objects.size() + shape.mesh.num_face_vertices.size());

        // For each face
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            const size_t fv = shape.mesh.num_face_vertices[f];
            assert(fv == 3);

            std::array<Eigen::Vector3f, 3> vertices = {};

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                const tinyobj::index_t& idx =
                    shape.mesh.indices[index_offset + v];

                vertices.at(v) = Eigen::Vector3f(
                    tinyobj_attrib.vertices[3 * idx.vertex_index + 0],
                    tinyobj_attrib.vertices[3 * idx.vertex_index + 1],
                    tinyobj_attrib.vertices[3 * idx.vertex_index + 2]);
            }

            const auto& material = materials[shape.mesh.material_ids[f]];

            objects.emplace_back(std::make_unique<Triangle>(
                std::make_tuple(vertices[0], vertices[1], vertices[2]),
                material));

            index_offset += fv;
        }
    }
    return objects;
}

static std::vector<std::unique_ptr<Geometry>> parse_geometries(
    const json& j,
    const std::unordered_map<std::string, std::shared_ptr<Material>>& materials,
    const std::filesystem::path& base_path) {
    if (!j.contains("objects")) {
        std::cerr << "No objects found in JSON file.\n";
        exit(EXIT_FAILURE);
    }
    const json& j_objs = j["objects"];

    std::vector<std::unique_ptr<Geometry>> objects;
    objects.reserve(j_objs.size());
    for (const json& jobj : j_objs) {
        const std::shared_ptr<Material> material =
            (jobj.contains("material") && materials.contains(jobj["material"]))
                ? materials.at(jobj["material"])
                : nullptr;

        if (jobj["type"] == "sphere") {
            objects.emplace_back(std::make_unique<Sphere>(
                to_vector3f(jobj["center"]), jobj["radius"].get<float>(),
                material));

        } else if (jobj["type"] == "triangle") {
            objects.emplace_back(std::make_unique<Triangle>(
                std::make_tuple(to_vector3f(jobj["corners"][0]),
                                to_vector3f(jobj["corners"][1]),
                                to_vector3f(jobj["corners"][2])),
                material));

        } else if (jobj["type"] == "stl") {
            auto stl_objects = read_stl(jobj, material, base_path);
            objects.reserve(objects.size() + stl_objects.size());
            std::ranges::move(stl_objects, std::back_inserter(objects));

        } else if (jobj["type"] == "obj") {
            auto obj_objects = read_obj(jobj, base_path);
            objects.reserve(objects.size() + obj_objects.size());
            std::ranges::move(obj_objects, std::back_inserter(objects));

        } else {
            std::cout << "Unknown object type: " << jobj["type"] << "\n";
        }
    }
    objects.shrink_to_fit();
    return objects;
}

}  // namespace

Scene read_json(const std::string& filename) {
    const std::filesystem::path base_path =
        std::filesystem::path(filename).parent_path();
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Error opening file: " << filename << '\n';
        exit(EXIT_FAILURE);
    }

    json j;
    infile >> j;

    auto materials = parse_materials(j);
    auto geometries = parse_geometries(j, materials, base_path);
    auto lights = parse_lights(j);
    auto camera = parse_camera(j);

    return {std::move(camera), std::move(geometries), std::move(lights)};
}
