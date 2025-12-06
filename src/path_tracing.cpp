/**
 * Monte Carlo Path Tracing.
 *
 * References:
 * https://sites.cs.ucsb.edu/~lingqi/teaching/resources/GAMES101_Lecture_15.pdf
 * https://computergraphics.stackexchange.com/questions/5152/progressive-path-tracing-with-explicit-light-sampling
 */

#include "path_tracing.h"

#include <Eigen/Core>

#include "Intersection.h"
#include "brdf.h"
#include "util/random.h"

namespace {

static const float PROBABILITY_SAMPLE_INDIRECT = 0.5F;

static const int MAX_BOUNCES = 128;
const float EPSILON = 1e-6F;
const float RAY_EPSILON = 1e-5F;

static Eigen::Vector3f path_trace(const Ray& ray, const Scene& scene,
                                  const int bounces) {
    if (bounces > MAX_BOUNCES) return Eigen::Vector3f::Zero();

    const Intersection intersection = scene.geometries->intersect(ray);
    if (!intersection.has_intersection()) return Eigen::Vector3f::Zero();

    const Eigen::Vector3f surface_point =
        ray.origin + intersection.t * ray.direction;
    Eigen::Vector3f normal = intersection.object->normal_at(ray, surface_point);
    const Eigen::Vector2f texcoords =
        intersection.object->texcoords_at(surface_point);

    // Apply normal mapping
    if (intersection.object->material->normal) {
        Eigen::Vector3f normal_local =
            intersection.object->material->normal->sample(texcoords);
        normal_local =
            ((2 * normal_local) - Eigen::Vector3f::Ones()).normalized();
        const Eigen::Matrix3f tbn =
            intersection.object->tangent_space_at(surface_point, normal);
        normal = (tbn * normal_local).normalized();
    }

    // Contribution from a light source
    const auto sample_direct = [&]() -> Eigen::Vector3f {
        // Sample a light or an emissive material
        const size_t num_lights = scene.emissive_objects.size();
        std::uniform_int_distribution<size_t> random_light(0, num_lights - 1);
        const auto light = scene.emissive_objects[random_light(rng)];

        Ray ray_to_light = light->ray_from(surface_point);
        const float distance = ray_to_light.max_t;

        // Check if the light is facing the surface
        const float cos_theta = normal.dot(ray_to_light.direction);
        if (cos_theta <= EPSILON) return Eigen::Vector3f::Zero();

        // Check for occlusion
        ray_to_light.min_t += RAY_EPSILON;
        ray_to_light.max_t -= RAY_EPSILON;
        const Intersection ray_to_light_intersection =
            scene.geometries->intersect(ray_to_light);
        if (ray_to_light_intersection.has_intersection())
            return Eigen::Vector3f::Zero();

        const float pdf = light->pdf(ray_to_light, distance);
        const auto brdf_value =
            brdf(ray, ray_to_light, intersection, normal, texcoords);

        // emission * brdf * cos_theta / (1 / pdf / num_lights)
        return light->emission_at(texcoords).cwiseProduct(brdf_value) *
               cos_theta * pdf * num_lights;
    };

    // Contribution from indirect lighting
    const auto sample_indirect = [&]() -> Eigen::Vector3f {
        Ray reflected_ray =
            brdf_sample(ray, intersection, surface_point, normal, texcoords);
        reflected_ray.min_t += RAY_EPSILON;

        const float cos_theta = normal.dot(reflected_ray.direction);
        if (cos_theta <= EPSILON) return Eigen::Vector3f::Zero();

        const auto brdf_value =
            brdf(ray, reflected_ray, intersection, normal, texcoords);
        const float pdf =
            brdf_pdf(ray, reflected_ray, intersection, normal, texcoords);

        // L * brdf * cos_theta / pdf
        return path_trace(reflected_ray, scene, bounces + 1)
                   .cwiseProduct(brdf_value) *
               cos_theta / (pdf + EPSILON);
    };

    Eigen::Vector3f color = sample_direct();

    // Russian roulette for indirect lighting
    std::uniform_real_distribution<float> uniform_dist(0.F, 1.F);
    if (uniform_dist(rng) < PROBABILITY_SAMPLE_INDIRECT) {
        color += sample_indirect() / PROBABILITY_SAMPLE_INDIRECT;
    }

    // Ignore emission for indirect bounces, since they are accounted for in
    // direct lighting sampling
    if (bounces == 0) {
        color += intersection.object->emission_at(texcoords);
    }

    color = color.cwiseMin(scene.options.ray_clamp);

    return color;
}

}  // namespace

Eigen::Vector3f sample(const Ray& ray, const Scene& scene) {
    return path_trace(ray, scene, 0);
}
