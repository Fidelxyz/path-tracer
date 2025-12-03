#include "Triangle.h"

#include <Eigen/Dense>

#include "../util/random.h"
#include "Eigen/Core"

static const float EPSILON = 1e-6F;

Triangle::Triangle(
    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> corners,
    std::shared_ptr<Material> material)
    : Geometry(AABB(get<0>(corners)
                        .cwiseMin(get<1>(corners))
                        .cwiseMin(get<2>(corners)),
                    get<0>(corners)
                        .cwiseMax(get<1>(corners))
                        .cwiseMax(get<2>(corners))),
               std::move(material)),
      corners(std::move(corners)) {
    const Eigen::Vector3f edge1 =
        std::get<1>(this->corners) - std::get<0>(this->corners);
    const Eigen::Vector3f edge2 =
        std::get<2>(this->corners) - std::get<0>(this->corners);
    const Eigen::Vector3f cross_product = edge1.cross(edge2);
    normal = cross_product.normalized();
    area = cross_product.norm() / 2;

    edges = {edge1, edge2};
}

Intersection Triangle::intersect(const Ray& ray) const {
    if (!bounding_box.intersect(ray)) return Intersection::NoIntersection();

    // Möller–Trumbore intersection algorithm
    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

    const auto& v0 = std::get<0>(corners);
    const auto& e1 = std::get<0>(edges);
    const auto& e2 = std::get<1>(edges);

    // As tested, leaving auto to lazy evaluate is faster.

    const auto h = ray.direction.cross(e2);
    const float det = e1.dot(h);
    if (std::abs(det) < EPSILON) return Intersection::NoIntersection();

    const float inv_det = 1.F / det;

    const auto s = ray.origin - v0;
    const float u = inv_det * s.dot(h);
    if (u < 0.F || u > 1.F) return Intersection::NoIntersection();

    const auto q = s.cross(e1);
    const float v = inv_det * ray.direction.dot(q);
    if (v < 0.F || u + v > 1.F) return Intersection::NoIntersection();

    const float t = inv_det * e2.dot(q);
    if (t < ray.min_t || t > ray.max_t) return Intersection::NoIntersection();

    return {this, t};
}

Eigen::Vector3f Triangle::normal_at(
    const Ray& ray, [[maybe_unused]] const Intersection& intersection) const {
    return ray.direction.dot(normal) < 0 ? normal : -normal;
}

Ray Triangle::ray_from(Eigen::Vector3f point) const {
    // Sample a random point on the triangle surface using barycentric
    // coordinates.
    std::uniform_real_distribution<float> uniform_dist(0.F, 1.F);
    float alpha = uniform_dist(rng);
    float beta = uniform_dist(rng);

    // Reflect the mirrored triangle region onto the original triangle.
    if (alpha + beta > 1.0F) {
        alpha = 1.0F - alpha;
        beta = 1.0F - beta;
    }

    const float gamma = 1.0F - alpha - beta;

    const auto target_point = alpha * std::get<0>(corners) +
                              beta * std::get<1>(corners) +
                              gamma * std::get<2>(corners);
    const Eigen::Vector3f diff = target_point - point;
    const Eigen::Vector3f direction = diff.normalized();
    const float distance = diff.norm();

    return {std::move(point), direction, 0.F, distance};
}

float Triangle::pdf(const Ray& ray, const float distance) const {
    const auto cos_theta = std::abs(normal.dot(ray.direction));
    return area * cos_theta / (distance * distance);
}
