#ifndef RAY_H
#define RAY_H

#include <Eigen/Core>

class Ray {
   public:
    Ray(const Eigen::Vector3f origin, const Eigen::Vector3f direction,
        float min_t = 0.f, float max_t = std::numeric_limits<float>::infinity())
        : origin(std::move(origin)),
          direction(std::move(direction)),
          min_t(min_t),
          max_t(max_t) {}

    const Eigen::Vector3f origin;
    // Not necessarily unit-length direction vector. (It is often useful to have
    // non-unit length so that origin+t*direction lands on a special point when
    // t=1.)
    const Eigen::Vector3f direction;
    const float min_t;
    const float max_t;
};

#endif
