#ifndef PLANE_H
#define PLANE_H

#include <Eigen/Core>

#include "Object.h"

class Plane : public Object {
   public:
    // Point on plane
    Eigen::Vector3f point;
    // Normal of plane
    Eigen::Vector3f normal;
    // Intersect plane with ray.
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
