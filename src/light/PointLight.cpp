#include "PointLight.h"

#include <numbers>

#include "../util/random.h"

PointLight::PointLight(Eigen::Vector3f intensity, Eigen::Vector3f position,
                       const float radius)
    : Light(std::move(intensity)),
      position(std::move(position)),
      radius(radius) {}

Ray PointLight::ray_from(Eigen::Vector3f point) const {
    // Sample a random point on the sphere surface
    // https://devforum.roblox.com/t/how-to-generate-a-random-rotation-and-much-more/1549051
    std::uniform_real_distribution<float> uniform_dist(0.F, 1.F);
    const float a = 2 * std::numbers::pi_v<float> * uniform_dist(rng);
    const float x = 2 * uniform_dist(rng) - 1;
    const float r = std::sqrt(1 - x * x);
    Eigen::Vector3f d = {x, r * std::cos(a), r * std::sin(a)};
    const auto target_point = this->position + radius * d;

    const Eigen::Vector3f diff = target_point - point;
    const Eigen::Vector3f direction = diff.normalized();
    const float distance = diff.norm();

    return {std::move(point), direction, 0.F, distance};
}

float PointLight::inv_pdf(const Ray& /*ray*/, const float distance) const {
    // Total radiance should not depend on the size of the light source,
    // so we fix area = 1.
    return 1 / (distance * distance);
}
