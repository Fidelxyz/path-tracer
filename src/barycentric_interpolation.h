#ifndef BARYCENTRIC_INTERPOLATION_H
#define BARYCENTRIC_INTERPOLATION_H

#include <Eigen/Core>
#include <tuple>

template <typename T>
T barycentric_interpolation(const std::tuple<T, T, T>& values,
                            const Eigen::Vector3f& barycentric_coords) {
    return std::get<0>(values) * barycentric_coords(0) +
           std::get<1>(values) * barycentric_coords(1) +
           std::get<2>(values) * barycentric_coords(2);
}

#endif
