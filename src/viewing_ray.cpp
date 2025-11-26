#include "viewing_ray.h"

Ray viewing_ray(const Camera& camera, const int i, const int j, const int width,
                const int height) {
    const float u =
        ((static_cast<float>(j) + 0.5F) / static_cast<float>(width) - 0.5F) *
        camera.width;
    const float v =
        -(((static_cast<float>(i) + 0.5F) / static_cast<float>(height)) -
          0.5F) *
        camera.height;
    const Eigen::Vector3f direction =
        camera.u * u + camera.v * v - camera.w * camera.focal_length;
    return {camera.pos, direction};
}
