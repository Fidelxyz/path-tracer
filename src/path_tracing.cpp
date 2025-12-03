/**
 * Monte Carlo Path Tracing.
 *
 * References:
 * https://sites.cs.ucsb.edu/~lingqi/teaching/resources/GAMES101_Lecture_15.pdf
 * https://computergraphics.stackexchange.com/questions/5152/progressive-path-tracing-with-explicit-light-sampling
 */

#include "path_tracing.h"

#include <Eigen/Core>
#include <numbers>

#include "Intersection.h"
#include "brdf.h"
#include "util/random.h"

namespace {

static const float PROBABILITY_SAMPLE_INDIRECT = 0.5F;

static const int MAX_BOUNCES = 128;
const float RAY_EPSILON = 1e-5F;

static Eigen::Vector3f spherical_to_cartesian(const float theta,
                                              const float phi) {
    return {
        std::sin(theta) * std::cos(phi),
        std::sin(theta) * std::sin(phi),
        std::cos(theta),
    };
}

/**
 * Transform a vector from tangent space to world space given the normal.
 */
static Eigen::Vector3f from_tangent_space(const Eigen::Vector3f& vector,
                                          const Eigen::Vector3f& normal) {
    // https://learnopengl.com/Advanced-Lighting/Normal-Mapping

    // Choose an arbitrary vector that is not parallel to the normal
    const Eigen::Vector3f up = std::abs(normal.z()) < 0.99F
                                   ? Eigen::Vector3f::UnitZ()
                                   : Eigen::Vector3f::UnitY();
    const Eigen::Vector3f tangent = normal.cross(up).normalized();
    const Eigen::Vector3f bitangent = normal.cross(tangent).normalized();
    Eigen::Matrix3f tbn;
    tbn << tangent, bitangent, normal;
    return tbn * vector;
}

static Eigen::Vector3f sample_hemisphere(const Eigen::Vector3f& normal) {
    // Cosine-weighted hemisphere sampling
    // https://ameye.dev/notes/sampling-the-hemisphere/
    std::uniform_real_distribution<float> uniform_dist(0.F, 1.F);
    const float r1 = uniform_dist(rng);
    const float r2 = uniform_dist(rng);

    const float theta = std::acos(std::sqrt(1 - r1));
    const float phi = 2.F * std::numbers::pi_v<float> * r2;

    return from_tangent_space(spherical_to_cartesian(theta, phi), normal);
};

static Eigen::Vector3f path_trace(const Ray& ray, const Scene& scene,
                                  const int bounces) {
    if (bounces >= MAX_BOUNCES) return Eigen::Vector3f::Zero();

    const Intersection intersection = scene.geometries->intersect(ray);
    if (!intersection.has_intersection()) return Eigen::Vector3f::Zero();

    const Eigen::Vector3f normal =
        intersection.object->normal_at(ray, intersection);
    Eigen::Vector3f surface_point = ray.origin + intersection.t * ray.direction;

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
        if (cos_theta <= 0.F) return Eigen::Vector3f::Zero();

        // Check for occlusion
        ray_to_light.min_t += RAY_EPSILON;
        ray_to_light.max_t -= RAY_EPSILON;
        const Intersection ray_to_light_intersection =
            scene.geometries->intersect(ray_to_light);
        if (ray_to_light_intersection.has_intersection())
            return Eigen::Vector3f::Zero();

        const float pdf = light->pdf(ray_to_light, distance);
        const auto brdf_value = brdf(ray, ray_to_light, intersection, normal);

        // emission * brdf * cos_theta / (1 / pdf / num_lights)
        return light->emission().cwiseProduct(brdf_value) * cos_theta * pdf *
               num_lights;
    };

    // Contribution from indirect lighting
    const auto sample_indirect = [&]() -> Eigen::Vector3f {
        const Ray reflected_ray = {surface_point, sample_hemisphere(normal),
                                   RAY_EPSILON};

        const auto brdf_value = brdf(ray, reflected_ray, intersection, normal);

        // pdf = cos_theta / pi  (cosine-weighted hemisphere sampling)
        // L * brdf * cos_theta / (cos_theta / pi)
        return path_trace(reflected_ray, scene, bounces + 1)
                   .cwiseProduct(brdf_value) *
               std::numbers::pi_v<float>;
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
        color += intersection.object->emission();
    }

    return color;
}

}  // namespace

Eigen::Vector3f sample(const Ray& ray, const Scene& scene) {
    return path_trace(ray, scene, 0);
}
