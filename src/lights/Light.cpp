#include "Light.h"

Light::Light(const Eigen::Vector3f intensity)
    : intensity(std::move(intensity)) {}
