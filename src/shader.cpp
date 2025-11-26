#include "shader.h"

#include <Eigen/src/Core/Matrix.h>

#include <numbers>

const float SHADOW_RAY_EPSILON = 1e-5;

Eigen::Vector3f shading(const Ray& ray, const Intersection& intersection,
                        const Eigen::Vector3f& normal, const Scene& scene) {
    const Eigen::Vector3f shading_pos =
        ray.origin + intersection.t * ray.direction;
    const Eigen::Vector2f uv =
        intersection.object->texcoord_at(ray, intersection);

    const Eigen::Vector3f diffuse_color =
        intersection.object->material->diffuse->sample(uv);
    const Eigen::Vector3f specular_color =
        intersection.object->material->specular->sample(uv);
    const float roughness =
        intersection.object->material->roughness->sample(uv);

    const Eigen::Vector3f v = -ray.direction.normalized();
    const float cos_theta_v = normal.dot(v);

    // Ambient
    Eigen::Vector3f rgb = intersection.object->material->ambient->sample(uv);

    for (const auto& light : scene.lights) {
        const Ray ray_to_light =
            light->ray_from(shading_pos + SHADOW_RAY_EPSILON * normal);
        const Eigen::Vector3f& l = ray_to_light.direction;

        // Shadow
        const Intersection shadow_intersection =
            scene.objects->intersect(ray_to_light);
        if (shadow_intersection.has_intersection()) continue;

        const Eigen::Vector3f h = (l + v).normalized();
        const float cos_theta_l = normal.dot(l);
        const float cos_theta_d = l.dot(h);

        // TODO: pow or mutliplication

        // Diffuse
        const float f_d90 = 0.5F + 2.F * roughness * std::pow(cos_theta_d, 2.F);
        const auto diffuse =
            diffuse_color / std::numbers::pi_v<float> *
            (1.F + (f_d90 - 1.F) * std::pow(1.F - cos_theta_l, 5.F)) *
            (1.F + (f_d90 - 1.F) * std::pow(1.F - cos_theta_v, 5.F));

        // // Specular
        // const auto specular =
        //     intersection.object->material->ks.cwiseProduct(light->intensity)
        //     * std::pow(std::max(0.F, normal.dot(h)),
        //              intersection.object->material->shininess);

        rgb += diffuse;
    }

    return rgb;
}
