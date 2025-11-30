#include "viewing_ray.h"

#include "util/random.h"

Ray viewing_ray(const Camera& camera, const int i, const int j, const int width,
                const int height) {
    std::uniform_real_distribution<float> dist(0.F, 1.F);

    const float x = static_cast<float>(j) + dist(rng);
    const float y = static_cast<float>(i) + dist(rng);

    const float u = (x / static_cast<float>(width) - 0.5F) * camera.width;
    const float v = -(y / static_cast<float>(height) - 0.5F) * camera.height;

    const Eigen::Vector3f direction =
        (camera.u * u + camera.v * v - camera.w * camera.focal_length)
            .normalized();
    return {camera.position, direction};
}
