#ifndef READ_TEXTURE_H
#define READ_TEXTURE_H

#include <stb_image.h>

#include <Eigen/Core>
#include <filesystem>
#include <iostream>
#include <limits>
#include <span>

#include "../material/Constant.h"
#include "../material/Texture.h"
#include "gamma_transform.h"

template <typename T, float gamma>
static T read_img_u8(const std::span<unsigned char> data_span, size_t i) {
    if constexpr (std::is_same_v<T, float>) {
        const float value =
            static_cast<float>(data_span[i * CHANNELS<T>]) /
            static_cast<float>(std::numeric_limits<unsigned char>::max());
        return apply_gamma<float, gamma>(value);

    } else if constexpr (std::is_same_v<T, Eigen::Vector3f>) {
        const Eigen::Vector3f color =
            Eigen::Vector3<unsigned int>(data_span[i * CHANNELS<T> + 0],
                                         data_span[i * CHANNELS<T> + 1],
                                         data_span[i * CHANNELS<T> + 2])
                .cast<float>() /
            static_cast<float>(std::numeric_limits<unsigned char>::max());
        return apply_gamma<Eigen::Vector3f, gamma>(color);

    } else {
        static_assert("Unsupported type for read_u8().");
    }
}

template <typename T, float gamma>
std::unique_ptr<Sampler<T>> read_texture(const std::filesystem::path& path,
                                         T&& default_value) {
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* const data_raw =
        stbi_load(path.c_str(), &width, &height, &channels, CHANNELS<T>);
    if (!data_raw) {
        std::cerr << "Failed to load texture: " << path << "\n";
        return std::make_unique<Constant<T>>(std::forward<T>(default_value));
    }
    const std::span<unsigned char> data_span(
        data_raw, static_cast<size_t>(width * height * CHANNELS<T>));
    const auto num_pixels =
        static_cast<size_t>(width) * static_cast<size_t>(height);

    std::vector<T> data;
    data.reserve(num_pixels);
    for (size_t i = 0; i < num_pixels; i++) {
        data.emplace_back(read_img_u8<T, gamma>(data_span, i));
    }

    auto texture = std::make_unique<Texture<T>>(width, height, data);

    stbi_image_free(data_raw);
    return texture;
}

#endif
