#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H
#include <Eigen/Core>

#include "Light.h"

class DirectionalLight : public Light {
   public:
    DirectionalLight(Eigen::Vector3f intensity, Eigen::Vector3f direction);

    Ray ray_from(Eigen::Vector3f point) const override;

    // Direction _from_ light toward scene.
    const Eigen::Vector3f direction;
};
#endif
