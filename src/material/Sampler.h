#ifndef SAMPLER_H
#define SAMPLER_H

#include <Eigen/Core>

template <typename T>
class Sampler {
   public:
    Sampler() = default;
    Sampler(const Sampler&) = delete;
    Sampler(Sampler&&) = delete;
    Sampler& operator=(const Sampler&) = delete;
    Sampler& operator=(Sampler&&) = delete;
    virtual ~Sampler() = default;

    [[nodiscard]] virtual T sample(const Eigen::Vector2f& uv) const = 0;
};

using SamplerGray = Sampler<float>;
using SamplerRGB = Sampler<Eigen::Vector3f>;

template <typename T>
inline constexpr int CHANNELS = 0;
template <>
inline constexpr int CHANNELS<float> = 1;
template <>
inline constexpr int CHANNELS<Eigen::Vector3f> = 3;

#endif
