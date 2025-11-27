#include "PointLight.h"

#include <random>

#include "../util/random.h"
#include "Light.h"

PointLight::PointLight(Eigen::Vector3f intensity, Eigen::Vector3f position,
                       const float radius)
    : Light(std::move(intensity)),
      position(std::move(position)),
      radius(radius) {}

Ray PointLight::ray_from(const Eigen::Vector3f point) const {
    // Sample a random point on the sphere surface
    // https://mathworld.wolfram.com/SpherePointPicking.html
    std::normal_distribution<float> normal_dist(0.F, 1.F);
    const Eigen::Vector3f random_dir = {normal_dist(rng), normal_dist(rng),
                                        normal_dist(rng)};
    const auto target_point = this->position + radius * random_dir.normalized();

    const Eigen::Vector3f diff = target_point - point;
    const Eigen::Vector3f direction = diff.normalized();
    const float max_t = diff.norm();

    return {point, direction, SHADOW_RAY_EPSILON, max_t - SHADOW_RAY_EPSILON};
}

float PointLight::angular_size_from([[maybe_unused]] const Ray& ray,
                                    const float distance) const {
    // Approximate the angular size
    return 1.F / (distance * distance);
}
