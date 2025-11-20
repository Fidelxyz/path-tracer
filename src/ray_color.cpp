#include "ray_color.h"

#include "Intersection.h"
#include "blinn_phong_shading.h"

const int MAX_REFLECTION = 16;

namespace {
Eigen::Vector3f reflect(const Eigen::Vector3f& in, const Eigen::Vector3f& n) {
    return in - 2 * in.dot(n) * n;
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
            intersection.object->intersection_normal(ray, intersection.t);

        rgb = blinn_phong_shading(ray, intersection, normal, scene);

        // Reflection
        const Ray reflected_ray = {ray.origin + intersection.t * ray.direction,
                                   reflect(ray.direction, normal)};
        if (Eigen::Vector3f indirect;
            ray_color_recursive(reflected_ray, depth + 1, indirect)) {
            rgb += intersection.object->material->km.cwiseProduct(indirect);
        }

        return true;
    };

    Eigen::Vector3f rgb;
    if (!ray_color_recursive(ray, 0, rgb)) {
        return Eigen::Vector3f::Zero();
    }
    return rgb;
}
