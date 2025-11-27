#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <Eigen/Core>
#include <cassert>

#include "Object.h"

class Triangle : public Object {
   public:
    Triangle(
        std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> corners,
        std::shared_ptr<Material> material);

    [[nodiscard]] Intersection intersect(const Ray& ray) const override;

    [[nodiscard]] Eigen::Vector3f normal_at(
        const Ray& ray, const Intersection& intersection) const override;

    [[nodiscard]] const Eigen::Vector3f& intensity() const override {
        assert(material);
        return material->emission;
    }

    [[nodiscard]] Ray ray_from(Eigen::Vector3f point) const override;

    [[nodiscard]] float angular_size_from(const Ray& ray,
                                          float distance) const override;

    // A triangle has three corners
    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> corners;
    // Precomputed normal vector
    Eigen::Vector3f normal;
    // Precomputed area
    float area;
};

#endif
