#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <Eigen/Core>

#include "Object.h"

class Triangle : public Object {
   public:
    Triangle(
        std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> corners,
        std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> normals,
        std::tuple<Eigen::Vector2f, Eigen::Vector2f, Eigen::Vector2f> texcoords,
        std::shared_ptr<Material> material);

    Triangle(
        std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> corners,
        std::shared_ptr<Material> material);

    [[nodiscard]] Intersection intersect(const Ray& ray) const override;

    [[nodiscard]] Eigen::Vector3f normal_at(
        const Ray& ray, const Intersection& intersection) const override;

    [[nodiscard]] Eigen::Vector2f texcoord_at(
        const Ray& ray, const Intersection& intersection) const override;

    // A triangle has three corners
    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> corners;
    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> normals;
    std::tuple<Eigen::Vector2f, Eigen::Vector2f, Eigen::Vector2f> texcoords;
};

#endif
