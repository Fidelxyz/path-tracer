#include "blinn_phong_shading.h"

#include <Eigen/src/Core/Matrix.h>

#include "first_hit.h"

Eigen::Vector3f blinn_phong_shading(
    const Ray& ray, const int& hit_id, const double& t,
    const Eigen::Vector3f& n,
    const std::vector<std::shared_ptr<Object>>& objects,
    const std::vector<std::shared_ptr<Light>>& lights) {
    const Object& obj = *objects[hit_id];
    const auto shading_pos = ray.origin + t * ray.direction;

    // Ambient
    Eigen::Vector3f rgb = obj.material->ka * .1;

    for (auto& l : lights) {
        const Light& light = *l;

        Eigen::Vector3f light_dir;
        double max_t;
        light.direction(shading_pos, light_dir, max_t);

        // Shadow
        const Ray shadow_ray = {shading_pos + 1e-6 * n, light_dir};
        int shadow_hit_id;
        double shadow_t;
        Eigen::Vector3f shadow_n;
        bool hit = first_hit(shadow_ray, 1e-6, objects, shadow_hit_id, shadow_t,
                             shadow_n);
        if (hit && shadow_t < max_t) continue;

        // Diffuse
        rgb += obj.material->kd.cwiseProduct(light.I) *
               std::max(0.f, n.dot(light_dir));

        // Specular
        const auto h = (light_dir - ray.direction.normalized()).normalized();
        rgb += obj.material->ks.cwiseProduct(light.I) *
               std::pow(std::max(0.f, n.dot(h)), obj.material->phong_exponent);
    }

    return rgb;
}
