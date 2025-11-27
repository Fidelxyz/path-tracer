#include "brdf.h"

#include <Eigen/src/Core/Matrix.h>

#include "object/Object.h"

Eigen::Vector3f brdf(const Ray& view_to_surface, const Ray& ray_to_light,
                     const Intersection& view_intersection,
                     const Eigen::Vector3f& normal) {
    const auto& material = view_intersection.object->material;

    // Diffuse
    const auto diffuse =
        material->diffuse * std::max(0.F, normal.dot(ray_to_light.direction));

    // Specular
    const auto h =
        (ray_to_light.direction - view_to_surface.direction.normalized())
            .normalized();
    const auto specular =
        material->specular *
        std::pow(std::max(0.F, normal.dot(h)), material->shininess);

    return diffuse + specular;
}
