#include "viewing_ray.h"

void viewing_ray(const Camera& camera, const int i, const int j,
                 const int width, const int height, Ray& out_ray) {
    const double u = ((j + 0.5) / width - 0.5) * camera.width;
    const double v = -((i + 0.5) / height - 0.5) * camera.height;
    const Eigen::Vector3f direction =
        camera.u * u + camera.v * v - camera.w * camera.d;
    out_ray = {camera.e, direction};
}
