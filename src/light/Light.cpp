#include "Light.h"

Light::Light(Eigen::Vector3f intensity) : intensity(std::move(intensity)) {}
