#include "PointLight.h"

PointLight::PointLight(const Eigen::Vector3f intensity,
                       const Eigen::Vector3f position)
    : Light(std::move(intensity)), position(std::move(position)) {}

Ray PointLight::ray_from(const Eigen::Vector3f point) const {
    const Eigen::Vector3f diff = this->position - point;

    const Eigen::Vector3f direction = diff.normalized();
    const float max_t = diff.norm();

    return Ray(point, direction, 0.f, max_t);
}
