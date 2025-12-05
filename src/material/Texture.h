#ifndef TEXTURE_H
#define TEXTURE_H

#include "Sampler.h"

template <typename T>
class Texture : public Sampler<T> {
   public:
    Texture(const unsigned int width, const unsigned int height,
            std::vector<T> data)
        : width(width), height(height), data(std::move(data)) {
        data.shrink_to_fit();
    }

    [[nodiscard]] T sample(const Eigen::Vector2f& uv) const override {
        const float x = uv.x() * width - 0.5F;
        const float y = uv.y() * height - 0.5F;

        int x0 = static_cast<int>(std::floor(x));
        int y0 = static_cast<int>(std::floor(y));

        const float tx = x - static_cast<float>(x0);
        const float ty = y - static_cast<float>(y0);

        x0 %= width;
        y0 %= height;
        if (x0 < 0) x0 += width;
        if (y0 < 0) y0 += height;

        const int x1 = (x0 + 1) % width;
        const int y1 = (y0 + 1) % height;

        const T& c00 = data[y0 * width + x0];
        const T& c10 = data[y0 * width + x1];
        const T& c01 = data[y1 * width + x0];
        const T& c11 = data[y1 * width + x1];

        return (1 - ty) * ((1 - tx) * c00 + tx * c10) +
               ty * ((1 - tx) * c01 + tx * c11);
    }

   private:
    unsigned int width;
    unsigned int height;
    std::vector<T> data;
};

using TextureGray = Texture<float>;
using TextureRGB = Texture<Eigen::Vector3f>;

#endif
