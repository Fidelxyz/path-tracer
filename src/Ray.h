#ifndef RAY_H
#define RAY_H

#include <Eigen/Core>

class Ray {
   public:
    Ray(Eigen::Vector3f origin, Eigen::Vector3f direction,
        const float min_t = 0.F,
        const float max_t = std::numeric_limits<float>::infinity())
        : origin(std::move(origin)),
          direction(std::move(direction)),
          min_t(min_t),
          max_t(max_t),
          inv_direction(this->direction.cwiseInverse()) {}

    // Origin point of the ray.
    Eigen::Vector3f origin;
    // Unit direction vector.
    Eigen::Vector3f direction;
    float min_t;
    float max_t;

    // Precomputed multiplication inverse of direction for fast AABB
    // intersection.
    Eigen::Vector3f inv_direction;
};

#endif
