#include "AABB.h"

AABB::AABB()
    : min_corner(Eigen::Vector3f::Constant(Eigen::Infinity)),
      max_corner(Eigen::Vector3f::Constant(-Eigen::Infinity)) {}

AABB::AABB(Eigen::Vector3f min_corner, Eigen::Vector3f max_corner)
    : min_corner(std::move(min_corner)), max_corner(std::move(max_corner)) {}

void AABB::merge(const AABB& other) {
    min_corner = min_corner.cwiseMin(other.min_corner);
    max_corner = max_corner.cwiseMax(other.max_corner);
}

Eigen::Vector3f AABB::center() const { return (min_corner + max_corner) / 2; }

Eigen::Vector3f AABB::dimensions() const { return max_corner - min_corner; }

bool AABB::intersect(const Ray& ray) const {
    const Eigen::Vector3f t1 =
        (min_corner - ray.origin).cwiseQuotient(ray.direction);
    const Eigen::Vector3f t2 =
        (max_corner - ray.origin).cwiseQuotient(ray.direction);

    const Eigen::Vector3f t_min_vec = t1.cwiseMin(t2);
    const Eigen::Vector3f t_max_vec = t1.cwiseMax(t2);

    const float t_min = std::max(t_min_vec.maxCoeff(), ray.min_t);
    const float t_max = std::min(t_max_vec.minCoeff(), ray.max_t);

    return t_min <= t_max;
}
