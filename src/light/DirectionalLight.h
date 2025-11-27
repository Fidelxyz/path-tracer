#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H
#include <Eigen/Core>

#include "Light.h"

class DirectionalLight : public Light {
   public:
    DirectionalLight(Eigen::Vector3f intensity, Eigen::Vector3f direction);

    [[nodiscard]] Ray ray_from(Eigen::Vector3f point) const override;

    [[nodiscard]] float angular_size_from(const Ray& ray,
                                          float distance) const override;

    // Direction _from_ light toward scene.
    Eigen::Vector3f direction;
};
#endif
