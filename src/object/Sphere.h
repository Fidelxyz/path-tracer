#ifndef SPHERE_H
#define SPHERE_H

#include <Eigen/Core>

#include "Object.h"

class Sphere : public Object {
   public:
    Sphere(Eigen::Vector3f center, float radius,
           std::shared_ptr<Material> material);

    [[nodiscard]] Intersection intersect(const Ray& ray) const override;

    [[nodiscard]] Eigen::Vector3f normal_at(
        const Ray& ray, const Intersection& intersection) const override;

    Eigen::Vector3f center;
    float radius;
};

#endif
