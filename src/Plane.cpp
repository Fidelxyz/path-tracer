#include "Plane.h"

#include "Ray.h"

bool Plane::intersect(const Ray& ray, const double min_t, double& out_t,
                      Eigen::Vector3d& out_n) const {
    const double t = (this->point - ray.origin).dot(this->normal) /
                     ray.direction.dot(this->normal);

    if (t < min_t ||
        t == std::numeric_limits<
                 std::remove_reference_t<decltype(out_t)>>::infinity())
        return false;

    Eigen::Vector3d n = this->normal;
    if (n.dot(ray.direction) > 0) n = -n;

    out_t = t;
    out_n = std::move(n);
    return true;
}
