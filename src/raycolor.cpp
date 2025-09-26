#include "raycolor.h"

#include "blinn_phong_shading.h"
#include "first_hit.h"
#include "reflect.h"

bool raycolor(const Ray& ray, const double min_t,
              const std::vector<std::shared_ptr<Object> >& objects,
              const std::vector<std::shared_ptr<Light> >& lights,
              const int num_recursive_calls, Eigen::Vector3d& out_rgb) {
    if (num_recursive_calls >= 16) return false;

    int hit_id;
    double t;
    Eigen::Vector3d n;
    if (!first_hit(ray, min_t, objects, hit_id, t, n)) return false;
    const Object& obj = *objects[hit_id];

    Eigen::Vector3d rgb =
        blinn_phong_shading(ray, hit_id, t, n, objects, lights);

    // Reflection
    const Ray reflected_ray = {ray.origin + t * ray.direction,
                               reflect(ray.direction, n)};
    Eigen::Vector3d indirect;
    if (raycolor(reflected_ray, 1e-6, objects, lights, num_recursive_calls + 1,
                 indirect)) {
        rgb += obj.material->km.cwiseProduct(indirect);
    }

    out_rgb = std::move(rgb);
    return true;
}
