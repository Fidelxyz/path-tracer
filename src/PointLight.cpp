#include "PointLight.h"

void PointLight::direction(const Eigen::Vector3f& q, Eigen::Vector3f& d,
                           double& max_t) const {
    const Eigen::Vector3f diff = this->p - q;

    d = diff.normalized();
    max_t = diff.norm();
}
