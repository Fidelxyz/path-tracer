#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Core>
#include <Eigen/Geometry>

#include "Ray.h"

class Camera {
   public:
    Camera(Eigen::Vector3f position, const Eigen::Vector3f& rotation,
           float focal_length, float width, float height,
           unsigned int resolution_x, unsigned int resolution_y,
           float exposure);

    [[nodiscard]] Ray viewing_ray(unsigned int i, unsigned int j) const;

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
    // Exposure.
    float exposure;
};

#endif
