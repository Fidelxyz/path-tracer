#include "blinn_phong_shading.h"

#include <Eigen/src/Core/Matrix.h>

const float SHADOW_RAY_EPSILON = 1e-5;

Eigen::Vector3f blinn_phong_shading(const Ray& ray,
                                    const Intersection& intersection,
                                    const Eigen::Vector3f& normal,
                                    const Scene& scene) {
    const Eigen::Vector3f shading_pos =
        ray.origin + intersection.t * ray.direction;

    // Ambient
    Eigen::Vector3f rgb = intersection.object->material->ka;

    for (auto& light : scene.lights) {
        const Ray ray_to_light =
            light->ray_from(shading_pos + SHADOW_RAY_EPSILON * normal);

        // Shadow
        const Intersection shadow_intersection =
            scene.objects->intersect(ray_to_light);
        if (shadow_intersection.has_intersection()) continue;

        // Diffuse
        const auto diffuse =
            intersection.object->material->kd.cwiseProduct(light->intensity) *
            std::max(0.f, normal.dot(ray_to_light.direction));

        // Specular
        const auto h =
            (ray_to_light.direction - ray.direction.normalized()).normalized();
        const auto specular =
            intersection.object->material->ks.cwiseProduct(light->intensity) *
            std::pow(std::max(0.f, normal.dot(h)),
                     intersection.object->material->phong_exponent);

        rgb += diffuse + specular;
    }

    return rgb;
}
