#include "read_json.h"

#include <igl/readSTL.h>

#include <Eigen/Core>
#include <fstream>

#define JSON_DIAGNOSTICS 1  // Enable extended diagnostic messages
#include <json.hpp>

#include "../geometry/Sphere.h"
#include "../geometry/Triangle.h"
#include "../light/DirectionalLight.h"
#include "../light/PointLight.h"
#include "gamma_transform.h"
#include "read_obj.h"
#include "read_texture.h"

using json = nlohmann::json;

namespace Eigen {

void from_json(const json& j, Vector3f& v) {
    j.at(0).get_to(v[0]);
    j.at(1).get_to(v[1]);
    j.at(2).get_to(v[2]);
}

}  // namespace Eigen

namespace {

static Options parse_options(const json& j) {
    const json& j_opts = j.at("options");
    return Options{
        .samples = j_opts.at("samples"),
        .ray_clamp =
            j_opts.value("ray_clamp", std::numeric_limits<float>::infinity()),
    };
}

static Camera parse_camera(const json& j) {
    if (!j.contains("camera")) {
        std::cerr << "No camera found in JSON file.\n";
        exit(EXIT_FAILURE);
    }
    const json& j_cam = j.at("camera");

    assert(j_cam.at("type") == "perspective" &&
           "Only handling perspective cameras");
    const Eigen::Vector3f position = j_cam.at("position");
    const Eigen::Vector3f rotation = j_cam.at("rotation");
    const float focal_length = j_cam.at("focal_length");
    const float height = j_cam.at("height");
    const float width = j_cam.at("width");
    const unsigned int resolution_x = j_cam.at("resolution_x");
    const unsigned int resolution_y = j_cam.at("resolution_y");
    const float exposure = j_cam.value("exposure", 1.F);
    return {position, rotation,     focal_length, width,
            height,   resolution_x, resolution_y, exposure};
}

static std::vector<std::unique_ptr<Light>> parse_lights(const json& j) {
    if (!j.contains("lights")) return {};
    const json& j_lights = j.at("lights");

    std::vector<std::unique_ptr<Light>> lights;
    lights.reserve(j_lights.size());
    for (const json& j_light : j_lights) {
        const Eigen::Vector3f intensity =
            j_light.at("intensity") *
            j_light.at("color").get<Eigen::Vector3f>();

        if (j_light.at("type") == "directional") {
            lights.emplace_back(std::make_unique<DirectionalLight>(
                intensity,
                j_light.at("direction").get<Eigen::Vector3f>().normalized()));

        } else if (j_light.at("type") == "point") {
            lights.emplace_back(std::make_unique<PointLight>(
                intensity, j_light.at("position"), j_light.at("radius")));

        } else {
            std::cout << "Unknown light type: " << j_light.at("type") << "\n";
        }
    }
    lights.shrink_to_fit();
    return lights;
}

template <typename T, float gamma>
static std::unique_ptr<Sampler<T>> parse_sampler(
    const json& jmat, const std::string_view key,
    const std::filesystem::path& base_path, T&& default_value) {
    if (jmat.contains(key) && jmat[key].is_string()) {
        return read_texture<T, gamma>(base_path / jmat[key],
                                      std::forward<T>(default_value));
    } else {
        return std::make_unique<Constant<T>>(
            jmat.value<T>(key, std::forward<T>(default_value)));
    }
};

static std::unordered_map<std::string, std::shared_ptr<Material>>
parse_materials(const json& j, const std::filesystem::path& base_path) {
    if (!j.contains("materials")) return {};
    const json& j_materials = j.at("materials");

    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
    materials.reserve(j_materials.size());
    for (const json& jmat : j_materials) {
        const std::string name = jmat.at("name");

        auto diffuse = parse_sampler<Eigen::Vector3f, GAMMA_SRGB>(
            jmat, "diffuse", base_path, Eigen::Vector3f::Ones());
        auto emission = parse_sampler<Eigen::Vector3f, GAMMA_LINEAR>(
            jmat, "emission", base_path, Eigen::Vector3f::Zero());

        const bool emissive = jmat.contains("emission") &&
                              (jmat["emission"].is_string() ||
                               jmat["emission"].get<Eigen::Vector3f>().any());

        auto roughness = parse_sampler<float, GAMMA_LINEAR>(jmat, "roughness",
                                                            base_path, 1.F);
        auto metallic = parse_sampler<float, GAMMA_LINEAR>(jmat, "metallic",
                                                           base_path, 0.F);

        materials[name] = std::make_shared<Material>(
            Material{.diffuse = std::move(diffuse),
                     .emission = std::move(emission),
                     .roughness = std::move(roughness),
                     .metallic = std::move(metallic),
                     .emissive = emissive});
    }
    return materials;
}

static std::vector<std::unique_ptr<Geometry>> read_stl(
    const json& jobj, const std::shared_ptr<Material>& material,
    const std::filesystem::path& base_path) {
    std::ifstream stl_file(base_path / jobj.at("stl"), std::ios::binary);
    if (!stl_file) {
        std::cerr << "Error opening STL file: " << jobj.at("stl") << '\n';
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
            std::array<Eigen::Vector3f, 3>{V.row(f[0]), V.row(f[1]),
                                           V.row(f[2])},
            std::array<Eigen::Vector3f, 3>{N.row(f[0]), N.row(f[1]),
                                           N.row(f[2])},
            std::array<Eigen::Vector2f, 3>{Eigen::Vector2f::Zero(),
                                           Eigen::Vector2f::Zero(),
                                           Eigen::Vector2f::Zero()},
            material));
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
    const json& j_objs = j.at("objects");

    std::vector<std::unique_ptr<Geometry>> objects;
    objects.reserve(j_objs.size());
    for (const json& jobj : j_objs) {
        const std::shared_ptr<Material> material =
            (jobj.contains("material") && materials.contains(jobj["material"]))
                ? materials.at(jobj["material"])
                : nullptr;

        if (jobj.at("type") == "sphere") {
            objects.emplace_back(std::make_unique<Sphere>(
                jobj.at("center"), jobj.at("radius"), material));

        } else if (jobj.at("type") == "triangle") {
            const std::array<Eigen::Vector3f, 3> vertices = {
                jobj.at("corners")[0], jobj.at("corners")[1],
                jobj.at("corners")[2]};
            const Eigen::Vector3f normal =
                ((vertices[1] - vertices[0]).cross(vertices[2] - vertices[0]))
                    .normalized();
            objects.emplace_back(std::make_unique<Triangle>(
                vertices, std::array{normal, normal, normal},
                std::array<Eigen::Vector2f, 3>{Eigen::Vector2f::Zero(),
                                               Eigen::Vector2f::Zero(),
                                               Eigen::Vector2f::Zero()},
                material));

        } else if (jobj.at("type") == "stl") {
            auto stl_objects = read_stl(jobj, material, base_path);
            objects.reserve(objects.size() + stl_objects.size());
            std::ranges::move(stl_objects, std::back_inserter(objects));

        } else if (jobj.at("type") == "obj") {
            auto obj_objects = read_obj(base_path / jobj.at("obj"));
            objects.reserve(objects.size() + obj_objects.size());
            std::ranges::move(obj_objects, std::back_inserter(objects));

        } else {
            std::cout << "Unknown object type: " << jobj.at("type") << "\n";
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

    auto options = parse_options(j);
    auto camera = parse_camera(j);
    auto materials = parse_materials(j, base_path);
    auto geometries = parse_geometries(j, materials, base_path);
    auto lights = parse_lights(j);

    return {options, std::move(camera), std::move(geometries),
            std::move(lights)};
}
