#include "AABB.h"

AABB::AABB()
    : min_corner(Eigen::Vector3f::Constant(Eigen::Infinity)),
      max_corner(Eigen::Vector3f::Constant(-Eigen::Infinity)) {}

AABB::AABB(Eigen::Vector3f min_corner, Eigen::Vector3f max_corner)
    : min_corner(min_corner), max_corner(max_corner) {}

void AABB::merge(const AABB& other) {
    min_corner = min_corner.cwiseMin(other.min_corner);
    max_corner = max_corner.cwiseMax(other.max_corner);
}

Eigen::Vector3f AABB::center() const { return (min_corner + max_corner) / 2.f; }

Eigen::Vector3f AABB::dimensions() const { return max_corner - min_corner; }

bool AABB::intersect(const Ray& ray) const {
    float t_xmin = (min_corner.x() - ray.origin.x()) / ray.direction.x();
    float t_ymin = (min_corner.y() - ray.origin.y()) / ray.direction.y();
    float t_zmin = (min_corner.z() - ray.origin.z()) / ray.direction.z();
    float t_xmax = (max_corner.x() - ray.origin.x()) / ray.direction.x();
    float t_ymax = (max_corner.y() - ray.origin.y()) / ray.direction.y();
    float t_zmax = (max_corner.z() - ray.origin.z()) / ray.direction.z();

    if (t_xmin > t_xmax) std::swap(t_xmin, t_xmax);
    if (t_ymin > t_ymax) std::swap(t_ymin, t_ymax);
    if (t_zmin > t_zmax) std::swap(t_zmin, t_zmax);

    const double t_min = std::max({t_xmin, t_ymin, t_zmin, ray.min_t});
    const double t_max = std::min({t_xmax, t_ymax, t_zmax, ray.max_t});

    return t_min <= t_max;
}
