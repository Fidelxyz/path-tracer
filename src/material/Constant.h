#ifndef CONSTANT_H
#define CONSTANT_H

#include "Sampler.h"

template <typename T>
class Constant : public Sampler<T> {
   public:
    explicit Constant(T value) : value(std::move(value)) {}

    [[nodiscard]] T sample(const Eigen::Vector2f& /*uv*/) const override {
        return value;
    }

   private:
    T value;
};

using ConstantGray = Constant<float>;
using ConstantRGB = Constant<Eigen::Vector3f>;

#endif
