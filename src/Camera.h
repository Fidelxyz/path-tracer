#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Core>

class Camera {
   public:
    Camera(Eigen::Vector3f e, Eigen::Vector3f u, Eigen::Vector3f v,
           Eigen::Vector3f w, float d, float width, float height)
        : e(std::move(e)),
          u(std::move(u)),
          v(std::move(v)),
          w(std::move(w)),
          d(std::move(d)),
          width(width),
          height(height) {}

    // Origin or "eye"
    const Eigen::Vector3f e;
    // orthonormal frame so that -w is the viewing direction.
    const Eigen::Vector3f u, v, w;
    // image plane distance / focal length
    const float d;
    // width and height of image plane
    const float width, height;
};

#endif
