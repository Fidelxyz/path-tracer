#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Core>

#include "Ray.h"

class Camera {
   public:
    Camera(Eigen::Vector3f position, const Eigen::Vector3f& rotation,
           float focal_length, float width, float height,
           unsigned int resolution_x, unsigned int resolution_y,
           float exposure);

    [[nodiscard]] Ray viewing_ray(unsigned int i, unsigned int j) const;

    Eigen::Vector3f position;   // Camera position.
    Eigen::Vector3f u;          // The up vector.
    Eigen::Vector3f v;          // The right vector.
    Eigen::Vector3f w;          // The backward vector.
    float focal_length;         // Focal length.
    float width;                // Width of image plane.
    float height;               // Height of image plane.
    unsigned int resolution_x;  // Horizontal resolution in pixels.
    unsigned int resolution_y;  // Vertical resolution in pixels.
    float exposure;             // Exposure.
};

#endif
