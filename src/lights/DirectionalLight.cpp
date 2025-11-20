#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Eigen::Vector3f intensity,
                                   Eigen::Vector3f direction)
    : Light(std::move(intensity)), direction(direction.normalized()) {}

Ray DirectionalLight::ray_from(const Eigen::Vector3f point) const {
    return {point, -direction};
}
