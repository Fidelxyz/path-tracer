#include "viewing_ray.h"

Ray viewing_ray(const Camera& camera, const int i, const int j, const int width,
                const int height) {
    const float u = ((j + 0.5f) / width - 0.5f) * camera.width;
    const float v = -((i + 0.5f) / height - 0.5f) * camera.height;
    const Eigen::Vector3f direction =
        camera.u * u + camera.v * v - camera.w * camera.d;
    return {camera.e, direction};
}
