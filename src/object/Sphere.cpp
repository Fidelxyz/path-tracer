#include "Sphere.h"

#include <numbers>
#include <optional>

#include "../Ray.h"

Sphere::Sphere(Eigen::Vector3f center, const float radius,
               std::shared_ptr<Material> material)
    : Object(std::move(material),
             AABB(center - Eigen::Vector3f::Constant(radius),
                  center + Eigen::Vector3f::Constant(radius))),
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

        if (discriminant < 0) return std::nullopt;

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

    const std::optional<float> t_opt = calc_t(ray);
    if (!t_opt.has_value()) return Intersection::NoIntersection();

    return {this, t_opt.value()};
}

Eigen::Vector3f Sphere::normal_at(const Ray& ray,
                                  const Intersection& intersection) const {
    Eigen::Vector3f n =
        (ray.origin + intersection.t * ray.direction - this->center)
            .normalized();
    if (n.dot(ray.direction) > 0) n = -n;
    return n;
}

Eigen::Vector2f Sphere::texcoord_at(const Ray& ray,
                                    const Intersection& intersection) const {
    const Eigen::Vector3f p =
        (ray.origin + intersection.t * ray.direction - this->center)
            .normalized();
    const float u =
        .5F + (std::atan2(p.z(), p.x()) / (2 * std::numbers::pi_v<float>));
    const float v = .5F - (std::asin(p.y()) / std::numbers::pi_v<float>);
    return {u, v};
}
