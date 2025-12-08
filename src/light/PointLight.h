#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

class PointLight : public Light {
   public:
    PointLight(Eigen::Vector3f intensity, Eigen::Vector3f position,
               float radius);

    [[nodiscard]] Ray ray_from(Eigen::Vector3f point) const override;

    [[nodiscard]] float inv_pdf(const Ray& ray,
                                const float distance) const override;

    Eigen::Vector3f position;
    float radius;
};
#endif
