#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Core>

class Camera {
   public:
    Camera(Eigen::Vector3f pos, Eigen::Vector3f u, Eigen::Vector3f v,
           Eigen::Vector3f w, const float focal_length, const float width,
           const float height, const float exposure)
        : pos(std::move(pos)),
          u(std::move(u)),
          v(std::move(v)),
          w(std::move(w)),
          focal_length(focal_length),
          width(width),
          height(height),
          exposure(exposure) {}

    // Camera position.
    Eigen::Vector3f pos;
    // Orthonormal frame so that -w is the viewing direction.
    Eigen::Vector3f u, v, w;
    // Focal length.
    float focal_length;
    // Width and height of image plane.
    float width, height;
    // Exposure.
    float exposure;
};

#endif
