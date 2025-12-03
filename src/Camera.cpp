#include "Camera.h"

#include <numbers>

#include "util/random.h"

Camera::Camera(Eigen::Vector3f position, const Eigen::Vector3f& rotation,
               const float focal_length, const float width, const float height,
               const unsigned int resolution_x, const unsigned int resolution_y,
               const unsigned int samples, const float exposure)
    : position(std::move(position)),
      focal_length(focal_length),
      width(width),
      height(height),
      resolution_x(resolution_x),
      resolution_y(resolution_y),
      samples(samples),
      exposure(exposure) {
    const float pitch = rotation.x() * std::numbers::pi_v<float> / 180.F;
    const float yaw = rotation.y() * std::numbers::pi_v<float> / 180.F;
    const float roll = rotation.z() * std::numbers::pi_v<float> / 180.F;

    Eigen::Matrix3f rot_matrix;
    rot_matrix = Eigen::AngleAxisf(roll, Eigen::Vector3f::UnitZ()) *
                 Eigen::AngleAxisf(yaw, Eigen::Vector3f::UnitY()) *
                 Eigen::AngleAxisf(pitch, Eigen::Vector3f::UnitX());

    v = rot_matrix * Eigen::Vector3f::UnitY();
    w = rot_matrix * Eigen::Vector3f::UnitZ();
    u = v.cross(w);
}

Ray Camera::viewing_ray(const unsigned int i, const unsigned int j) const {
    const auto res_w = static_cast<float>(resolution_x);
    const auto res_h = static_cast<float>(resolution_y);

    std::uniform_real_distribution<float> dist(0.F, 1.F);
    const float pixel_x = static_cast<float>(j) + dist(rng);
    const float pixel_y = static_cast<float>(i) + dist(rng);

    const float pixel_u = (pixel_x / res_w - 0.5F) * width;
    const float pixel_v = -(pixel_y / res_h - 0.5F) * height;

    const Eigen::Vector3f direction =
        (u * pixel_u + v * pixel_v - w * focal_length).normalized();
    return {position, direction};
}
