#include "Triangle.h"

#include <Eigen/Dense>

#include "Ray.h"

bool Triangle::intersect(const Ray& ray, const double min_t, double& out_t,
                         Eigen::Vector3d& out_n) const {
    const Eigen::Vector3d& x1 = std::get<0>(this->corners);
    const Eigen::Vector3d t1 =
        std::get<1>(this->corners) - std::get<0>(this->corners);
    const Eigen::Vector3d t2 =
        std::get<2>(this->corners) - std::get<0>(this->corners);

    Eigen::Matrix3d a;
    a << t1, t2, -ray.direction;
    const Eigen::Vector3d b = ray.origin - x1;

    const Eigen::Vector3d x = a.colPivHouseholderQr().solve(b);
    const double alpha = x(0);
    const double beta = x(1);
    const double t = x(2);

    if (t < min_t) return false;
    if (alpha < 0 || beta < 0) return false;
    if (alpha + beta > 1) return false;

    Eigen::Vector3d n = t1.cross(t2).normalized();
    if (n.dot(ray.direction) > 0) n = -n;

    out_t = t;
    out_n = std::move(n);
    return true;
}
