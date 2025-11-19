#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <Eigen/Core>

#include "Object.h"

class Triangle : public Object {
   public:
    // A triangle has three corners
    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> corners;
    // Intersect a triangle with ray.
    //
    // Inputs:
    //   Ray  ray to intersect with
    //   min_t  minimum parametric distance to consider
    // Outputs:
    //   t  first intersection at ray.origin + t * ray.direction
    //   n  surface normal at point of intersection
    // Returns iff there a first intersection is found.
    bool intersect(const Ray& ray, const double min_t, double& t,
                   Eigen::Vector3f& n) const;
};

#endif
