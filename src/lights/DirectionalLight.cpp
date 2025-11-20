#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(const Eigen::Vector3f intensity,
                                   const Eigen::Vector3f direction)
    : Light(std::move(intensity)), direction(direction.normalized()) {}

Ray DirectionalLight::ray_from(const Eigen::Vector3f point) const {
    return Ray(point, -direction);
}
