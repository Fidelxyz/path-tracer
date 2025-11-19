#ifndef RAY_H
#define RAY_H

#include <Eigen/Core>

struct Ray {
    Eigen::Vector3f origin;
    // Not necessarily unit-length direction vector. (It is often useful to have
    // non-unit length so that origin+t*direction lands on a special point when
    // t=1.)
    Eigen::Vector3f direction;
};

#endif
