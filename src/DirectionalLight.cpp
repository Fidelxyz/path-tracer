#include "DirectionalLight.h"

#include <limits>

void DirectionalLight::direction([[maybe_unused]] const Eigen::Vector3f& q,
                                 Eigen::Vector3f& out_d,
                                 double& out_max_t) const {
    out_d = -this->d;
    out_max_t = std::numeric_limits<
        std::remove_reference_t<decltype(out_max_t)>>::infinity();
}
