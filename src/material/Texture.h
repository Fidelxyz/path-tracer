#ifndef TEXTURE_H
#define TEXTURE_H

#include <Eigen/Core>
#include <vector>

#include "Sampler.h"

template <typename T>
class Texture : public Sampler<T> {
   public:
    Texture(std::vector<T> data, const int width, const int height)
        : data(std::move(data)), width(width), height(height) {}

    [[nodiscard]] T sample(const Eigen::Vector2f& uv) const override {
        // TODO: interpolation
        int u = static_cast<int>(uv.x() * width) % width;
        int v = static_cast<int>(uv.y() * height) % height;

        return data[v * width + u];
    }

   private:
    std::vector<T> data;
    unsigned int width;
    unsigned int height;
};

using TextureGray = Texture<float>;
using TextureRGB = Texture<Eigen::Vector3f>;

#endif
