#include "Triangle.h"

#include <Eigen/Dense>

#include "../Ray.h"

Triangle::Triangle(Eigen::Vector3f v0, Eigen::Vector3f v1, Eigen::Vector3f v2,
                   std::shared_ptr<Material> material)
    : Object(std::move(material),
             AABB(v0.cwiseMin(v1).cwiseMin(v2), v0.cwiseMax(v1).cwiseMax(v2))),
      corners(std::move(v0), std::move(v1), std::move(v2)) {}

Intersection Triangle::intersect(const Ray& ray) const {
    if (!bounding_box.intersect(ray)) return Intersection::NoIntersection();

    const auto& x1 = std::get<0>(this->corners);
    const auto t1 = std::get<1>(this->corners) - std::get<0>(this->corners);
    const auto t2 = std::get<2>(this->corners) - std::get<0>(this->corners);

    Eigen::Matrix3f a;
    a << t1, t2, -ray.direction;
    const auto b = ray.origin - x1;  // auto for lazy evaluation

    const Eigen::Vector3f x = a.colPivHouseholderQr().solve(b);
    const float alpha = x(0);
    const float beta = x(1);
    const float t = x(2);

    if (t < ray.min_t || t > ray.max_t) return Intersection::NoIntersection();
    if (alpha < 0 || beta < 0) return Intersection::NoIntersection();
    if (alpha + beta > 1) return Intersection::NoIntersection();

    return {this, t};
}

Eigen::Vector3f Triangle::intersection_normal(
    const Ray& ray, [[maybe_unused]] const float t) const {
    // auto for lazy evaluation
    const auto t1 = std::get<1>(this->corners) - std::get<0>(this->corners);
    const auto t2 = std::get<2>(this->corners) - std::get<0>(this->corners);
    Eigen::Vector3f n = t1.cross(t2).normalized();
    if (n.dot(ray.direction) > 0) n = -n;
    return n;
}
