#include "ray_color.h"

#include <optional>

#include "Intersection.h"
#include "shader.h"
#include "util/random.h"

namespace {

static const int MAX_REFLECTION = 16;

/**
 * Return a random reflected direction within the hemisphere around the normal.
 */
Eigen::Vector3f reflect(const Eigen::Vector3f& n) {
    const float a = 2 * std::numbers::pi_v<float> * uniform(rng);
    const float x = 2 * uniform(rng) - 1;
    const float r = std::sqrt(1 - x * x);
    Eigen::Vector3f d = {x, r * std::cos(a), r * std::sin(a)};
    if (d.dot(n) < 0) d = -d;
    return d;
}

std::optional<Eigen::Vector3f> path_trace(const Ray& ray, const Scene& scene,
                                          const int depth) {
    if (depth >= MAX_REFLECTION) return std::nullopt;

    const Intersection intersection = scene.objects->intersect(ray);
    if (!intersection.has_intersection()) return std::nullopt;

    const Eigen::Vector3f normal =
        intersection.object->normal_at(ray, intersection);

    // Reflection
    const Ray reflected_ray = {ray.origin + intersection.t * ray.direction,
                               reflect(normal)};

    Eigen::Vector3f brdf = shading(ray, intersection, normal, scene);

    // const auto indirect = path_trace(reflected_ray, scene, depth + 1);
    // if (indirect) {
    //     brdf += intersection.object->material->km.cwiseProduct(*indirect);
    // }

    return brdf;
}

}  // namespace

Eigen::Vector3f ray_color(const Ray& ray, const Scene& scene) {
    std::function<bool(const Ray&, const int, Eigen::Vector3f&)>
        ray_color_recursive =
            [&](const Ray& ray, const int depth, Eigen::Vector3f& rgb) -> bool {
        if (depth >= MAX_REFLECTION) return false;

        const Intersection intersection = scene.objects->intersect(ray);
        if (!intersection.has_intersection()) return false;

        const Eigen::Vector3f normal =
            intersection.object->normal_at(ray, intersection);

        rgb = shading(ray, intersection, normal, scene);

        // Reflection
        // const Ray reflected_ray = {ray.origin + intersection.t *
        // ray.direction,
        //                            reflect(ray.direction, normal)};
        // if (Eigen::Vector3f indirect;
        //     ray_color_recursive(reflected_ray, depth + 1, indirect)) {
        //     rgb += intersection.object->material->km.cwiseProduct(indirect);
        // }

        return true;
    };

    Eigen::Vector3f rgb;
    if (!ray_color_recursive(ray, 0, rgb)) {
        return Eigen::Vector3f::Zero();
    }
    return rgb;
}
