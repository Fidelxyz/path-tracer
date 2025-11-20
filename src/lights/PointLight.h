#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include <Eigen/Core>

#include "Light.h"

class PointLight : public Light {
   public:
    PointLight(Eigen::Vector3f intensity, Eigen::Vector3f position);

    Ray ray_from(Eigen::Vector3f point) const override;

    const Eigen::Vector3f position;
};
#endif
