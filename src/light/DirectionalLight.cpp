#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Eigen::Vector3f intensity,
                                   Eigen::Vector3f direction)
    : Light(std::move(intensity)), direction(direction.normalized()) {}

Ray DirectionalLight::ray_from(const Eigen::Vector3f point) const {
    return {point, -direction, SHADOW_RAY_EPSILON};
}

float DirectionalLight::angular_size_from(
    [[maybe_unused]] const Ray& ray,
    [[maybe_unused]] const float distance) const {
    return 1.F;
}
