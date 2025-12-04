#include "read_json.h"

#include <igl/readSTL.h>

#define JSON_DIAGNOSTICS 1  // Enable extended diagnostic messages
#include <tiny_obj_loader.h>

#include <Eigen/Core>
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

    const Eigen::Vector3f position = to_vector3f(j_cam.at("position"));
    const Eigen::Vector3f rotation = to_vector3f(j_cam.at("rotation"));
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
            j_light.at("intensity") * to_vector3f(j_light.at("color"));

        if (j_light.at("type") == "directional") {
            lights.emplace_back(std::make_unique<DirectionalLight>(
                intensity, to_vector3f(j_light.at("direction")).normalized()));

        } else if (j_light.at("type") == "point") {
            lights.emplace_back(std::make_unique<PointLight>(
                intensity, to_vector3f(j_light.at("position")),
                j_light.at("radius")));

        } else {
            std::cout << "Unknown light type: " << j_light.at("type") << "\n";
        }
    }
    lights.shrink_to_fit();
    return lights;
}

static std::unordered_map<std::string, std::shared_ptr<Material>>
parse_materials(const json& j) {
    if (!j.contains("materials")) return {};
    const json& j_materials = j.at("materials");

    std::unordered_map<std::string, std::shared_ptr<Material>> materials;
    materials.reserve(j_materials.size());
    for (const json& jmat : j_materials) {
        const std::string name = jmat.at("name");

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

        const float roughness = jmat.value("roughness", 1.F);
        const float metallic = jmat.value("metallic", 0.F);
        const float sheen = jmat.value("sheen", 0.F);
        const float clearcoat_thickness =
            jmat.value("clearcoat_thickness", 0.F);
        const float clearcoat_roughness =
            jmat.value("clearcoat_roughness", 0.F);
        const float anisotropy = jmat.value("anisotropy", 0.F);
        const float anisotropy_rotation =
            jmat.value("anisotropy_rotation", 0.F);

        materials[name] = std::make_shared<Material>(
            Material{.ambient = ambient,
                     .diffuse = diffuse,
                     .specular = specular,
                     .transmittance = transmittance,
                     .emission = emission,
                     .shininess = shininess,
                     .ior = ior,
                     .roughness = roughness,
                     .metallic = metallic,
                     .sheen = sheen,
                     .clearcoat_thickness = clearcoat_thickness,
                     .clearcoat_roughness = clearcoat_roughness,
                     .anisotropy = anisotropy,
                     .anisotropy_rotation = anisotropy_rotation,
                     .emissive = emission.any()});
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
            material));
    }

    return objects;
}

static std::vector<std::unique_ptr<Geometry>> read_obj(
    const json& jobj, const std::filesystem::path& base_path) {
    // Normal smoothing borrowed from:
    // https://github.com/tinyobjloader/tinyobjloader/blob/a4e519b0a0f29c790464fcfeadfe25a7f9fa15ff/examples/viewer/viewer.cc
    const auto compute_all_smoothing_normals =
        [](tinyobj::attrib_t& attrib, std::vector<tinyobj::shape_t>& shapes) {
            std::unordered_map<int, Eigen::Vector3f> normals;

            for (const auto& shape : shapes) {
                const size_t num_faces = shape.mesh.num_face_vertices.size();
                assert(shape.mesh.smoothing_group_ids.size());

                std::array<Eigen::Vector3f, 3> p = {};

                for (size_t f = 0; f < num_faces; f++) {
                    for (size_t v = 0; v < 3; v++) {
                        tinyobj::index_t idx = shape.mesh.indices[3 * f + v];
                        assert(idx.vertex_index != -1);
                        p.at(v) = Eigen::Vector3f(
                            attrib.vertices[3 * idx.vertex_index + 0],
                            attrib.vertices[3 * idx.vertex_index + 1],
                            attrib.vertices[3 * idx.vertex_index + 2]);
                    }

                    // Compute (unnormalized) face normal, area-weighted.
                    const Eigen::Vector3f normal =
                        (p[1] - p[0]).cross(p[2] - p[0]);

                    for (unsigned int v = 0; v < 3; ++v) {
                        tinyobj::index_t idx = shape.mesh.indices[3 * f + v];

                        const auto iter = normals.find(idx.vertex_index);
                        if (iter == normals.end()) {
                            normals[idx.normal_index] = normal;
                        } else {
                            iter->second += normal;
                        }
                    }
                }
            }

            assert(attrib.normals.size() % 3 == 0);
            for (auto& [index, normal] : normals) {
                normal.normalize();

                attrib.normals[3 * index + 0] = normal.x();
                attrib.normals[3 * index + 1] = normal.y();
                attrib.normals[3 * index + 2] = normal.z();
            }
        };

    const auto compute_smoothing_shape =
        [](const tinyobj::attrib_t& in_attrib, const tinyobj::shape_t& in_shape,
           const std::vector<std::pair<unsigned int, size_t>>& sorted_ids,
           const unsigned int id_begin, const unsigned int id_end,
           std::vector<tinyobj::shape_t>& out_shapes,
           tinyobj::attrib_t& out_attrib) {
            const unsigned int smoothing_group_id = sorted_ids[id_begin].first;
            const bool has_materials = !in_shape.mesh.material_ids.empty();

            // Make a new shape from the set of faces in the range [idbegin,
            // idend).
            tinyobj::shape_t& out_shape = out_shapes.emplace_back();
            out_shape.name = in_shape.name;
            // Skip lines and points.

            std::unordered_map<unsigned int, int> remap;
            for (unsigned int id = id_begin; id < id_end; id++) {
                const size_t face = sorted_ids[id].second;

                // always triangles
                out_shape.mesh.num_face_vertices.push_back(3);
                if (has_materials) {
                    out_shape.mesh.material_ids.push_back(
                        in_shape.mesh.material_ids[face]);
                }
                out_shape.mesh.smoothing_group_ids.push_back(
                    smoothing_group_id);
                // Skip tags.

                for (size_t v = 0; v < 3; v++) {
                    const tinyobj::index_t& in_idx =
                        in_shape.mesh.indices[3 * face + v];
                    tinyobj::index_t out_idx = {};

                    assert(in_idx.vertex_index != -1);

                    const auto iter = remap.find(in_idx.vertex_index);
                    // Smooth group 0 disables smoothing so no shared vertices
                    // in that case.
                    if (smoothing_group_id != 0 && iter != remap.end()) {
                        out_idx.vertex_index = (*iter).second;
                        out_idx.normal_index = out_idx.vertex_index;
                        out_idx.texcoord_index = (in_idx.texcoord_index == -1)
                                                     ? -1
                                                     : out_idx.vertex_index;
                    } else {
                        assert(out_attrib.vertices.size() % 3 == 0);

                        const auto offset =
                            static_cast<int>(out_attrib.vertices.size() / 3);
                        out_idx.vertex_index = offset;
                        out_idx.normal_index = offset;
                        out_idx.texcoord_index =
                            (in_idx.texcoord_index == -1) ? -1 : offset;

                        out_attrib.vertices.push_back(
                            in_attrib.vertices[3 * in_idx.vertex_index + 0]);
                        out_attrib.vertices.push_back(
                            in_attrib.vertices[3 * in_idx.vertex_index + 1]);
                        out_attrib.vertices.push_back(
                            in_attrib.vertices[3 * in_idx.vertex_index + 2]);

                        out_attrib.normals.push_back(0.F);
                        out_attrib.normals.push_back(0.F);
                        out_attrib.normals.push_back(0.F);

                        if (in_idx.texcoord_index != -1) {
                            out_attrib.texcoords.push_back(
                                in_attrib
                                    .texcoords[2 * in_idx.texcoord_index + 0]);
                            out_attrib.texcoords.push_back(
                                in_attrib
                                    .texcoords[2 * in_idx.texcoord_index + 1]);
                        }
                        remap[in_idx.vertex_index] = offset;
                    }
                    out_shape.mesh.indices.push_back(out_idx);
                }
            }
        };

    const auto compute_smoothing_shapes =
        [&](const tinyobj::attrib_t& inattrib,
            const std::vector<tinyobj::shape_t>& in_shapes)
        -> std::tuple<std::vector<tinyobj::shape_t>, tinyobj::attrib_t> {
        std::vector<tinyobj::shape_t> out_shapes;
        tinyobj::attrib_t out_attrib;

        for (const auto& shape : in_shapes) {
            const auto num_faces = static_cast<unsigned int>(
                shape.mesh.smoothing_group_ids.size());
            assert(num_faces != 0);

            std::vector<std::pair<unsigned int, size_t>> sorted_ids;
            sorted_ids.reserve(num_faces);
            for (unsigned int i = 0; i < num_faces; i++) {
                sorted_ids.emplace_back(shape.mesh.smoothing_group_ids[i], i);
            }
            std::ranges::sort(sorted_ids);

            unsigned int active_id = sorted_ids[0].first;
            unsigned int id = active_id;
            unsigned int idbegin = 0;
            unsigned int idend = 0;
            // Faces are now bundled by smoothing group id, create shapes from
            // these.
            while (idbegin < num_faces) {
                // Find the end of the current smoothing group id block.
                while (active_id == id && ++idend < num_faces) {
                    id = sorted_ids[idend].first;
                }

                compute_smoothing_shape(inattrib, shape, sorted_ids, idbegin,
                                        idend, out_shapes, out_attrib);

                active_id = id;
                idbegin = idend;
            }
        }

        return {out_shapes, out_attrib};
    };

    const std::filesystem::path obj_file = base_path / jobj.at("obj");

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

    auto in_attrib = reader.GetAttrib();
    auto in_shapes = reader.GetShapes();
    auto in_materials = reader.GetMaterials();

    // Materials
    std::vector<std::shared_ptr<Material>> materials;
    materials.reserve(in_materials.size());
    for (auto& material : in_materials) {
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

    if (in_attrib.normals.empty()) {
        auto smoothed = compute_smoothing_shapes(in_attrib, in_shapes);
        in_shapes = std::get<0>(smoothed);
        in_attrib = std::get<1>(smoothed);
        compute_all_smoothing_normals(in_attrib, in_shapes);
    }

    std::vector<std::unique_ptr<Geometry>> objects;

    for (const auto& shape : in_shapes) {
        objects.reserve(objects.size() + shape.mesh.num_face_vertices.size());

        // For each face
        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
            const size_t fv = shape.mesh.num_face_vertices[f];
            assert(fv == 3);

            std::array<Eigen::Vector3f, 3> vertices = {};
            std::array<Eigen::Vector3f, 3> normals = {};

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                const tinyobj::index_t& idx =
                    shape.mesh.indices[index_offset + v];

                vertices.at(v) = Eigen::Vector3f(
                    in_attrib.vertices[3 * idx.vertex_index + 0],
                    in_attrib.vertices[3 * idx.vertex_index + 1],
                    in_attrib.vertices[3 * idx.vertex_index + 2]);

                normals.at(v) = Eigen::Vector3f(
                    in_attrib.normals[3 * idx.normal_index + 0],
                    in_attrib.normals[3 * idx.normal_index + 1],
                    in_attrib.normals[3 * idx.normal_index + 2]);
            }

            const auto& material = materials[shape.mesh.material_ids[f]];

            objects.emplace_back(
                std::make_unique<Triangle>(vertices, normals, material));

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
                to_vector3f(jobj.at("center")), jobj.at("radius"), material));

        } else if (jobj.at("type") == "triangle") {
            const std::array<Eigen::Vector3f, 3> vertices = {
                to_vector3f(jobj.at("corners")[0]),
                to_vector3f(jobj.at("corners")[1]),
                to_vector3f(jobj.at("corners")[2])};
            const Eigen::Vector3f normal =
                ((vertices[1] - vertices[0]).cross(vertices[2] - vertices[0]))
                    .normalized();
            objects.emplace_back(std::make_unique<Triangle>(
                vertices, std::array{normal, normal, normal}, material));

        } else if (jobj.at("type") == "stl") {
            auto stl_objects = read_stl(jobj, material, base_path);
            objects.reserve(objects.size() + stl_objects.size());
            std::ranges::move(stl_objects, std::back_inserter(objects));

        } else if (jobj.at("type") == "obj") {
            auto obj_objects = read_obj(jobj, base_path);
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
    auto materials = parse_materials(j);
    auto geometries = parse_geometries(j, materials, base_path);
    auto lights = parse_lights(j);

    return {options, std::move(camera), std::move(geometries),
            std::move(lights)};
}
