#include "DirectionalLight.h"

DirectionalLight::DirectionalLight(Eigen::Vector3f intensity,
                                   Eigen::Vector3f direction)
    : Light(std::move(intensity)), direction(direction.normalized()) {}

Ray DirectionalLight::ray_from(Eigen::Vector3f point) const {
    return {std::move(point), -direction};
}

float DirectionalLight::pdf([[maybe_unused]] const Ray& ray,
                            [[maybe_unused]] const float distance) const {
    return 1.F;
}
