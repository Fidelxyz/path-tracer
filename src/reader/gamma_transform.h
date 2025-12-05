#ifndef GAMMA_TRANSFORM_H
#define GAMMA_TRANSFORM_H

#include <Eigen/Core>

constexpr float GAMMA_SRGB = 2.2F;
constexpr float GAMMA_LINEAR = 1.0F;

template <typename T, float gamma>
T apply_gamma(const T& color) {
    if constexpr (std::is_same_v<T, float>) {
        return std::pow(color, gamma);
    } else if constexpr (std::is_same_v<T, Eigen::Vector3f>) {
        return color.cwisePow(gamma);
    } else {
        static_assert("Unsupported type for apply_gamma().");
    }
}

#endif
