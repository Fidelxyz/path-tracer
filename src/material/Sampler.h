#ifndef SAMPLER_H
#define SAMPLER_H

#include <Eigen/Core>

template <typename T>
class Sampler {
   public:
    virtual ~Sampler() = default;

    [[nodiscard]] virtual T sample(const Eigen::Vector2f& uv) const = 0;
};

using SamplerGray = Sampler<float>;
using SamplerRGB = Sampler<Eigen::Vector3f>;

#endif
