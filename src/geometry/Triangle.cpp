#include "Triangle.h"

#include <Eigen/Dense>

#include "../util/random.h"

static const float EPSILON = 1e-6F;

Triangle::Triangle(std::array<Eigen::Vector3f, 3> vertices,
                   std::array<Eigen::Vector3f, 3> normals,
                   std::array<Eigen::Vector2f, 3> texcoords,
                   std::shared_ptr<Material> material)
    : Geometry(AABB(vertices[0].cwiseMin(vertices[1]).cwiseMin(vertices[2]),
                    vertices[0].cwiseMax(vertices[1]).cwiseMax(vertices[2])),
               std::move(material)),
      vertices(std::move(vertices)),
      normals(std::move(normals)),
      texcoords(std::move(texcoords)) {
    Eigen::Vector3f edge1 = this->vertices[1] - this->vertices[0];
    Eigen::Vector3f edge2 = this->vertices[2] - this->vertices[0];
    area = edge1.cross(edge2).norm() / 2;

    // Precompute data for barycentric coordinates
    d00 = edge1.dot(edge1);
    d01 = edge1.dot(edge2);
    d11 = edge2.dot(edge2);
    inv_denom = 1.F / (d00 * d11 - d01 * d01);

    // Precompute tangent
    // https://learnopengl.com/Advanced-Lighting/Normal-Mapping
    const Eigen::Vector2f delta_uv1 = texcoords[1] - texcoords[0];
    const Eigen::Vector2f delta_uv2 = texcoords[2] - texcoords[0];
    const float f =
        1.0F / (delta_uv1.x() * delta_uv2.y() - delta_uv2.x() * delta_uv1.y());
    tangent =
        (f * (delta_uv2.y() * edge1 - delta_uv1.y() * edge2)).normalized();

    edges = {std::move(edge1), std::move(edge2)};
}

std::tuple<float, float, float> Triangle::barycentric_coordinates(
    const Eigen::Vector3f& p) const {
    const Eigen::Vector3f& t0 = edges[0];
    const Eigen::Vector3f& t1 = edges[1];
    const Eigen::Vector3f t2 = p - vertices[0];

    const float d20 = t2.dot(t0);
    const float d21 = t2.dot(t1);

    const float v = (d11 * d20 - d01 * d21) * inv_denom;
    const float w = (d00 * d21 - d01 * d20) * inv_denom;
    const float u = 1.0F - v - w;

    return {u, v, w};
}

Intersection Triangle::intersect(const Ray& ray) const {
    if (!bounding_box.intersect(ray)) return Intersection::NoIntersection();

    // Möller–Trumbore intersection algorithm
    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

    const auto& v0 = std::get<0>(vertices);
    const auto& e1 = std::get<0>(edges);
    const auto& e2 = std::get<1>(edges);

    // As tested, leaving auto to lazy evaluate is faster.

    const auto h = ray.direction.cross(e2);
    const float det = e1.dot(h);
    if (std::abs(det) < EPSILON) return Intersection::NoIntersection();

    const float inv_det = 1.F / det;

    const auto s = ray.origin - v0;
    const float v = inv_det * s.dot(h);
    if (v < 0.F || v > 1.F) return Intersection::NoIntersection();

    const auto q = s.cross(e1);
    const float w = inv_det * ray.direction.dot(q);
    if (w < 0.F || v + w > 1.F) return Intersection::NoIntersection();

    const float t = inv_det * e2.dot(q);
    if (t < ray.min_t || t > ray.max_t) return Intersection::NoIntersection();

    return {this, t};
}

Eigen::Vector3f Triangle::normal_at(const Ray& ray,
                                    const Eigen::Vector3f& point) const {
    const auto [u, v, w] = barycentric_coordinates(point);

    Eigen::Vector3f n =
        (u * normals[0] + v * normals[1] + w * normals[2]).normalized();
    if (n.dot(ray.direction) > 0.F) {
        n = -n;
    }
    return n;
}

Eigen::Matrix3f Triangle::tangent_space_at(
    const Eigen::Vector3f& /*point*/, const Eigen::Vector3f& normal) const {
    // re-orthogonalize the TBN vectors
    const Eigen::Vector3f tangent =
        (this->tangent - normal * normal.dot(this->tangent)).normalized();
    const Eigen::Vector3f bitangent = normal.cross(tangent).normalized();

    Eigen::Matrix3f tbn;
    tbn << tangent, bitangent, normal;
    return tbn;
}

Eigen::Vector2f Triangle::texcoords_at(const Eigen::Vector3f& point) const {
    const auto [u, v, w] = barycentric_coordinates(point);

    return u * texcoords[0] + v * texcoords[1] + w * texcoords[2];
}

Ray Triangle::ray_from(Eigen::Vector3f point) const {
    // Sample a random point on the triangle surface using barycentric
    // coordinates.
    std::uniform_real_distribution<float> uniform_dist(0.F, 1.F);
    float u = uniform_dist(rng);
    float v = uniform_dist(rng);

    // Reflect the mirrored triangle region onto the original triangle.
    if (u + v > 1.0F) {
        u = 1.0F - u;
        v = 1.0F - v;
    }

    const float w = 1.0F - u - v;

    const auto target_point =
        u * vertices[0] + v * vertices[1] + w * vertices[2];
    const Eigen::Vector3f diff = target_point - point;
    const Eigen::Vector3f direction = diff.normalized();
    const float distance = diff.norm();

    return {std::move(point), direction, 0.F, distance};
}

float Triangle::pdf(const Ray& ray, const float distance) const {
    const Eigen::Vector3f point = ray.origin + ray.direction * distance;
    const auto [u, v, w] = barycentric_coordinates(point);
    const auto normal =
        (u * normals[0] + v * normals[1] + w * normals[2]).normalized();

    const auto cos_theta = std::abs(normal.dot(ray.direction));
    return area * cos_theta / (distance * distance);
}
