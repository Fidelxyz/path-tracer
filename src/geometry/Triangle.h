#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "Geometry.h"

class Triangle : public Geometry {
   public:
    Triangle(std::array<Eigen::Vector3f, 3> vertices,
             std::array<Eigen::Vector3f, 3> normals,
             std::shared_ptr<Material> material);

    [[nodiscard]] Intersection intersect(const Ray& ray) const override;

    [[nodiscard]] Eigen::Vector3f normal_at(
        const Ray& ray, const Intersection& intersection) const override;

    [[nodiscard]] Ray ray_from(Eigen::Vector3f point) const override;

    [[nodiscard]] float pdf(const Ray& ray,
                            const float distance) const override;

    std::array<Eigen::Vector3f, 3> vertices;
    std::array<Eigen::Vector3f, 3> normals;

   private:
    [[nodiscard]] std::tuple<float, float, float> barycentric_coordinates(
        const Eigen::Vector3f& p) const;

    // Precomputed area
    float area;
    // Precomputed edges
    std::array<Eigen::Vector3f, 2> edges;
    // Precomputed data for barycentric_coordinates
    float d00, d01, d11, inv_denom;
};

#endif
