#ifndef LIGHT_H
#define LIGHT_H

#include "../Object.h"

class Light : public Object {
   public:
    explicit Light(Eigen::Vector3f intensity)
        : intensity(std::move(intensity)) {}
    Light(const Light&) = default;
    Light(Light&&) = delete;
    Light& operator=(const Light&) = default;
    Light& operator=(Light&&) = delete;
    ~Light() override = default;

    [[nodiscard]] Eigen::Vector3f emission_at(
        const Eigen::Vector2f& /*texcoords*/) const override {
        return intensity;
    }

    Eigen::Vector3f intensity;
};
#endif
