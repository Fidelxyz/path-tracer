#include "Light.h"

Light::Light(Eigen::Vector3f intensity) : intensity_(std::move(intensity)) {}

const Eigen::Vector3f& Light::intensity() const { return intensity_; }
