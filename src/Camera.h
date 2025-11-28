#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <numbers>

class Camera {
   public:
    Camera(Eigen::Vector3f position, Eigen::Vector3f rotation,
           const float focal_length, const float width, const float height,
           unsigned int resolution_x, unsigned int resolution_y,
           unsigned int samples, const float exposure)
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

    // Camera position.
    Eigen::Vector3f position;
    // Orthonormal frame so that -w is the viewing direction.
    Eigen::Vector3f u, v, w;
    // Focal length.
    float focal_length;
    // Width and height of image plane.
    float width, height;

    // Resolution of image plane.
    unsigned int resolution_x, resolution_y;
    // Number of samples per pixel.
    unsigned int samples;
    // Exposure.
    float exposure;
};

#endif
