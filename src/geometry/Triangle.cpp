#include "Triangle.h"

#include <Eigen/Dense>

#include "../util/random.h"

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
    const auto t1 = std::get<1>(this->corners) - std::get<0>(this->corners);
    const auto t2 = std::get<2>(this->corners) - std::get<0>(this->corners);
    const Eigen::Vector3f cross_product = t1.cross(t2);
    normal = cross_product.normalized();
    area = cross_product.norm() / 2;
}

Intersection Triangle::intersect(const Ray& ray) const {
    if (!bounding_box.intersect(ray)) return Intersection::NoIntersection();

    const auto& x1 = std::get<0>(this->corners);
    const auto t1 = std::get<1>(this->corners) - std::get<0>(this->corners);
    const auto t2 = std::get<2>(this->corners) - std::get<0>(this->corners);

    Eigen::Matrix3f a;
    a << t1, t2, -ray.direction;
    const auto b = ray.origin - x1;  // auto for lazy evaluation

    const Eigen::Vector3f x = a.partialPivLu().solve(b);

    const float t = x(2);
    if (t < ray.min_t || t > ray.max_t) return Intersection::NoIntersection();

    const float alpha = x(0);
    const float beta = x(1);
    const float gamma = 1.F - alpha - beta;
    if (alpha < 0 || beta < 0 || gamma < 0)
        return Intersection::NoIntersection();

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
