#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <Eigen/Core>

#include "Light.h"

class PointLight : public Light {
   public:
    PointLight(Eigen::Vector3f intensity, Eigen::Vector3f position,
               float radius);

    [[nodiscard]] Ray ray_from(Eigen::Vector3f point) const override;

    [[nodiscard]] float angular_size_from(const Ray& ray,
                                          float distance) const override;

    Eigen::Vector3f position;
    float radius;
};
#endif
