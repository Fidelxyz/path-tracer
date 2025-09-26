#include "PointLight.h"

void PointLight::direction(const Eigen::Vector3d& q, Eigen::Vector3d& d,
                           double& max_t) const {
    const Eigen::Vector3d diff = this->p - q;

    d = diff.normalized();
    max_t = diff.norm();
}
