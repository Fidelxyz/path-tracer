#ifndef SPHERE_H
#define SPHERE_H

#include "Geometry.h"

class Sphere : public Geometry {
   public:
    Sphere(Eigen::Vector3f center, float radius,
           std::shared_ptr<Material> material);

    [[nodiscard]] Intersection intersect(const Ray& ray) const override;

    [[nodiscard]] Eigen::Vector3f normal_at(
        const Ray& ray, const Eigen::Vector3f& point) const override;

    [[nodiscard]] Eigen::Vector2f texcoords_at(
        const Eigen::Vector3f& point) const override;

    [[nodiscard]] Ray ray_from(Eigen::Vector3f point) const override;

    [[nodiscard]] float pdf(const Ray& ray,
                            const float distance) const override;

    Eigen::Vector3f center;
    float radius;
};

#endif
