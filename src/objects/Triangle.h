#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <Eigen/Core>

#include "Object.h"

class Triangle : public Object {
   public:
    Triangle(const Eigen::Vector3f v0, const Eigen::Vector3f v1,
             const Eigen::Vector3f v2, std::shared_ptr<Material> material);

    [[nodiscard]] Intersection intersect(const Ray& ray) const override;

    [[nodiscard]] Eigen::Vector3f intersection_normal(const Ray& ray,
                                                      float t) const override;

    // A triangle has three corners
    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> corners;
};

#endif
