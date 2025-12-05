#include "Sphere.h"

#include <numbers>
#include <optional>

#include "../util/random.h"

static const float EPSILON = 1e-6F;

Sphere::Sphere(Eigen::Vector3f center, const float radius,
               std::shared_ptr<Material> material)
    : Geometry(AABB(center - Eigen::Vector3f::Constant(radius),
                    center + Eigen::Vector3f::Constant(radius)),
               std::move(material)),
      center(std::move(center)),
      radius(radius) {}

Intersection Sphere::intersect(const Ray& ray) const {
    if (!bounding_box.intersect(ray)) {
        return Intersection::NoIntersection();
    }

    const auto calc_t = [this](const Ray& ray) -> std::optional<float> {
        const float a = ray.direction.squaredNorm();
        const float b = 2 * ray.direction.dot(ray.origin - center);
        const float c = (ray.origin - center).squaredNorm() - (radius * radius);
        const float discriminant = (b * b) - (4 * a * c);

        if (discriminant < EPSILON) return std::nullopt;

        // smaller t
        float t = (-b - std::sqrt(discriminant)) / (2 * a);
        if (t > ray.max_t) return std::nullopt;
        if (t >= ray.min_t) return t;

        // larger t
        t = (-b + std::sqrt(discriminant)) / (2 * a);
        if (t > ray.max_t) return std::nullopt;
        if (t >= ray.min_t) return t;

        return std::nullopt;
    };

    const std::optional<float> t = calc_t(ray);
    if (!t) return Intersection::NoIntersection();

    return {this, *t};
}

Eigen::Vector3f Sphere::normal_at(const Ray& ray,
                                  const Intersection& intersection) const {
    Eigen::Vector3f n =
        (ray.origin + intersection.t * ray.direction - this->center)
            .normalized();
    if (n.dot(ray.direction) > 0) n = -n;
    return n;
}

Ray Sphere::ray_from(const Eigen::Vector3f point) const {
    // Sample a random point on the sphere surface
    // https://devforum.roblox.com/t/how-to-generate-a-random-rotation-and-much-more/1549051
    std::uniform_real_distribution<float> uniform_dist(0.F, 1.F);
    const float a = 2 * std::numbers::pi_v<float> * uniform_dist(rng);
    const float x = 2 * uniform_dist(rng) - 1;
    const float r = std::sqrt(1 - x * x);
    Eigen::Vector3f d = {x, r * std::cos(a), r * std::sin(a)};
    const auto target_point = this->center + radius * d;

    const Eigen::Vector3f diff = target_point - point;
    const Eigen::Vector3f direction = diff.normalized();
    const float distance = diff.norm();

    return {point, direction, 0.F, distance};
}

float Sphere::pdf(const Ray& /*ray*/, const float distance) const {
    return (std::numbers::pi_v<float> * radius * radius) /
           (distance * distance);
}
