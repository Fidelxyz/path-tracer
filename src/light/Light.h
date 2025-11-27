#ifndef LIGHT_H
#define LIGHT_H

#include <Eigen/Core>

#include "../LightSource.h"

const float SHADOW_RAY_EPSILON = 1e-5;

class Light : public LightSource {
   public:
    explicit Light(Eigen::Vector3f intensity);
    Light(const Light&) = default;
    Light(Light&&) = delete;
    Light& operator=(const Light&) = default;
    Light& operator=(Light&&) = delete;
    ~Light() override = default;

    [[nodiscard]] const Eigen::Vector3f& intensity() const override;

    Eigen::Vector3f intensity_;
};
#endif
