#ifndef SPHERE_H
#define SPHERE_H

#include <Eigen/Core>

#include "Object.h"

class Sphere : public Object {
   public:
    Sphere(Eigen::Vector3f center, float radius,
           std::shared_ptr<Material> material);

    Intersection intersect(const Ray& ray) const override;

    Eigen::Vector3f intersection_normal(const Ray& ray, float t) const override;

    const Eigen::Vector3f center;
    const float radius;
};

#endif
