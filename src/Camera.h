#ifndef CAMERA_H
#define CAMERA_H

#include <Eigen/Core>

struct Camera {
    // Origin or "eye"
    Eigen::Vector3f e;
    // orthonormal frame so that -w is the viewing direction.
    Eigen::Vector3f u, v, w;
    // image plane distance / focal length
    double d;
    // width and height of image plane
    double width, height;
};

#endif
