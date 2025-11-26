#ifndef COLOR_H
#define COLOR_H

#include <Eigen/Core>

#include "Sampler.h"

template <typename T>
class Color : public Sampler<T> {
   public:
    explicit Color(T color) : color(std::move(color)) {}

    [[nodiscard]] T sample(
        [[maybe_unused]] const Eigen::Vector2f& uv) const override {
        return color;
    }

   private:
    T color;
};

using ColorGray = Color<float>;
using ColorRGB = Color<Eigen::Vector3f>;

#endif
