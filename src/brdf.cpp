/**
 * Cook-Torrance BRDF.
 *
 * Reference:
 * https://learnopengl.com/PBR/Theory
 */

#include "brdf.h"

#include <Eigen/src/Core/Matrix.h>

#include "object/Object.h"

Eigen::Vector3f brdf(const Ray& view_to_surface, const Ray& ray_to_light,
                     const Intersection& view_intersection,
                     const Eigen::Vector3f& normal) {
    const auto& material = view_intersection.object->material;

    // Lambertian diffuse
    const auto diffuse = material->diffuse / std::numbers::pi_v<float>;

    return diffuse;
}
