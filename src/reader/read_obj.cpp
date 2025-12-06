#include "read_obj.h"

#include <tiny_obj_loader.h>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <cassert>
#include <exception>
#include <iostream>

#include "../geometry/Triangle.h"
#include "read_texture.h"

namespace {

template <typename T>
static Eigen::Vector3f to_vector3f(const T& x) {
    return Eigen::Vector3f(x[0], x[1], x[2]);
}

// Normal smoothing borrowed from:
// https://github.com/tinyobjloader/tinyobjloader/blob/a4e519b0a0f29c790464fcfeadfe25a7f9fa15ff/examples/viewer/viewer.cc
void compute_all_smoothing_normals(tinyobj::attrib_t& attrib,
                                   std::vector<tinyobj::shape_t>& shapes) {
    std::unordered_map<int, Eigen::Vector3f> normals;

    for (const auto& shape : shapes) {
        const size_t num_faces = shape.mesh.num_face_vertices.size();
        assert(shape.mesh.smoothing_group_ids.size());

        std::array<Eigen::Vector3f, 3> p = {};

        for (size_t f = 0; f < num_faces; f++) {
            for (size_t v = 0; v < 3; v++) {
                tinyobj::index_t idx = shape.mesh.indices[3 * f + v];
                assert(idx.vertex_index != -1);
                p.at(v) =
                    Eigen::Vector3f(attrib.vertices[3 * idx.vertex_index + 0],
                                    attrib.vertices[3 * idx.vertex_index + 1],
                                    attrib.vertices[3 * idx.vertex_index + 2]);
            }

            // Compute (unnormalized) face normal, area-weighted.
            const Eigen::Vector3f normal = (p[1] - p[0]).cross(p[2] - p[0]);

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
}

void compute_smoothing_shape(
    const tinyobj::attrib_t& in_attrib, const tinyobj::shape_t& in_shape,
    const std::vector<std::pair<unsigned int, size_t>>& sorted_ids,
    const unsigned int id_begin, const unsigned int id_end,
    std::vector<tinyobj::shape_t>& out_shapes, tinyobj::attrib_t& out_attrib) {
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
        out_shape.mesh.smoothing_group_ids.push_back(smoothing_group_id);
        // Skip tags.

        for (size_t v = 0; v < 3; v++) {
            const tinyobj::index_t& in_idx =
                in_shape.mesh.indices[3 * face + v];
            tinyobj::index_t out_idx = {};

            assert(in_idx.vertex_index != -1);

            const auto iter = remap.find(in_idx.vertex_index);
            // Smooth group 0 disables smoothing so no shared
            // vertices in that case.
            if (smoothing_group_id != 0 && iter != remap.end()) {
                out_idx.vertex_index = (*iter).second;
                out_idx.normal_index = out_idx.vertex_index;
                out_idx.texcoord_index =
                    (in_idx.texcoord_index == -1) ? -1 : out_idx.vertex_index;
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
                        in_attrib.texcoords[2 * in_idx.texcoord_index + 0]);
                    out_attrib.texcoords.push_back(
                        in_attrib.texcoords[2 * in_idx.texcoord_index + 1]);
                }
                remap[in_idx.vertex_index] = offset;
            }
            out_shape.mesh.indices.push_back(out_idx);
        }
    }
}

template <typename T, float gamma>
static std::unique_ptr<Texture<T>> parse_sampler(
    const std::filesystem::path& base_path, const std::string_view texture) {
    if (texture.empty()) return nullptr;
    return read_texture<T, gamma>(base_path / texture);
}

template <typename T, float gamma>
static std::unique_ptr<Sampler<T>> parse_sampler(
    const std::filesystem::path& base_path, const std::string_view texture,
    T constant) {
    if (texture.empty()) {
        return std::make_unique<Constant<T>>(std::move(constant));
    }
    return read_texture<T, gamma>(base_path / texture, std::move(constant));
}

}  // namespace

std::vector<std::unique_ptr<Geometry>> read_obj(
    const std::filesystem::path& obj_file) {
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
            // Faces are now bundled by smoothing group id, create shapes
            // from these.
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
    const auto base_path = obj_file.parent_path();
    std::vector<std::shared_ptr<Material>> materials;
    materials.reserve(in_materials.size());
    for (auto& material : in_materials) {
        materials.emplace_back(std::make_shared<Material>(Material{
            .diffuse = parse_sampler<Eigen::Vector3f, GAMMA_SRGB>(
                base_path, material.diffuse_texname,
                to_vector3f(material.diffuse)),
            .emission = parse_sampler<Eigen::Vector3f, GAMMA_SRGB>(
                base_path, material.emissive_texname,
                to_vector3f(material.emission)),
            .roughness = parse_sampler<float, GAMMA_LINEAR>(
                base_path, material.roughness_texname, material.roughness),
            .metallic = parse_sampler<float, GAMMA_LINEAR>(
                base_path, material.metallic_texname, material.metallic),
            .normal = parse_sampler<Eigen::Vector3f, GAMMA_LINEAR>(
                base_path, material.normal_texname),
            .emissive = material.emissive_texname != "" ||
                        to_vector3f(material.emission).any(),
        }));
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
            std::array<Eigen::Vector2f, 3> texcoords = {};

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

                if (idx.texcoord_index != -1) {
                    texcoords.at(v) = Eigen::Vector2f(
                        in_attrib.texcoords[2 * idx.texcoord_index + 0],
                        in_attrib.texcoords[2 * idx.texcoord_index + 1]);
                } else {
                    texcoords.at(v) = Eigen::Vector2f::Zero();
                }
            }

            const auto& material = materials[shape.mesh.material_ids[f]];

            objects.emplace_back(std::make_unique<Triangle>(
                vertices, normals, texcoords, material));

            index_offset += fv;
        }
    }
    return objects;
}
