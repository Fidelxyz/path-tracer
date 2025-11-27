/**
 * Monte Carlo Path Tracing.
 *
 * References:
 * https://sites.cs.ucsb.edu/~lingqi/teaching/resources/GAMES101_Lecture_15.pdf
 */

#include "path_tracing.h"

#include <numbers>
#include <random>

#include "Eigen/Core"
#include "Intersection.h"
#include "brdf.h"
#include "util/random.h"

namespace {

static const int MAX_BOUNCES = 16;

/**
 * Return a random scattered direction within the hemisphere around the normal.
 */
static Eigen::Vector3f scatter(const Eigen::Vector3f& n) {
    std::uniform_real_distribution<float> uniform_dist(0.F, 1.F);
    const float a = 2 * std::numbers::pi_v<float> * uniform_dist(rng);
    const float x = 2 * uniform_dist(rng) - 1;
    const float r = std::sqrt(1 - x * x);
    Eigen::Vector3f d = {x, r * std::cos(a), r * std::sin(a)};
    if (d.dot(n) < 0) d = -d;
    return d;
}

static Eigen::Vector3f path_trace(const Ray& ray, const Scene& scene,
                                  const int depth) {
    if (depth >= MAX_BOUNCES) return Eigen::Vector3f::Zero();

    const Intersection intersection = scene.objects->intersect(ray);
    if (!intersection.has_intersection()) return Eigen::Vector3f::Zero();

    const auto& material = intersection.object->material;
    const Eigen::Vector3f normal =
        intersection.object->normal_at(ray, intersection);
    const Eigen::Vector3f surface_point =
        ray.origin + intersection.t * ray.direction;

    // Contribution from a light source
    const auto sample_direct = [&]() -> Eigen::Vector3f {
        // Sample a light source or an emissive object
        assert(!scene.lights.empty() || !scene.emissive_objects.empty());
        std::uniform_int_distribution<size_t> random_light(
            0, scene.lights.size() + scene.emissive_objects.size() - 1);
        const size_t light_idx = random_light(rng);
        const LightSource* const light =
            light_idx < scene.lights.size()
                ? static_cast<LightSource*>(scene.lights[light_idx].get())
                : scene.emissive_objects[light_idx - scene.lights.size()];

        const Ray ray_to_light = light->ray_from(surface_point);

        // Check for occlusion
        const Intersection ray_to_light_intersection =
            scene.objects->intersect(ray_to_light);
        if (ray_to_light_intersection.has_intersection())
            return Eigen::Vector3f::Zero();

        const float cos_theta = normal.dot(ray_to_light.direction);
        const float p =
            1.F / light->angular_size_from(ray_to_light, ray_to_light.max_t);
        const auto brdf_value = brdf(ray, ray_to_light, intersection, normal);

        return light->intensity().cwiseProduct(brdf_value) * cos_theta / p;
    };

    // Contribution from indirect lighting
    const auto sample_indirect = [&]() -> Eigen::Vector3f {
        const Ray reflected_ray = {surface_point, scatter(normal),
                                   SHADOW_RAY_EPSILON};

        // Check for occlusion
        const Intersection reflected_ray_intersection =
            scene.objects->intersect(reflected_ray);
        if (!reflected_ray_intersection.has_intersection())
            return Eigen::Vector3f::Zero();

        const float cos_theta = normal.dot(reflected_ray.direction);
        // PDF for uniform hemisphere sampling
        const float p = 1.F / (2.F * std::numbers::pi_v<float>);
        const auto brdf_value = brdf(ray, reflected_ray, intersection, normal);

        return path_trace(reflected_ray, scene, depth + 1)
                   .cwiseProduct(brdf_value) *
               cos_theta / p;
    };

    // Emission from the surface itself
    const Eigen::Vector3f& emission = material->emission;

    return emission + sample_direct() + sample_indirect();
}

}  // namespace

Eigen::Vector3f sample(const Ray& ray, const Scene& scene) {
    return path_trace(ray, scene, 0);
}
