#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Geometry.h"

class Triangle : public Geometry {
   public:
    Triangle(
        std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> corners,
        std::shared_ptr<Material> material);

    [[nodiscard]] Intersection intersect(const Ray& ray) const override;

    [[nodiscard]] Eigen::Vector3f normal_at(
        const Ray& ray, const Intersection& intersection) const override;

    [[nodiscard]] Ray ray_from(Eigen::Vector3f point) const override;

    [[nodiscard]] float pdf(const Ray& ray,
                            const float distance) const override;

    // A triangle has three corners
    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> corners;
    // Precomputed normal vector
    Eigen::Vector3f normal;
    // Precomputed area
    float area;
};

#endif
