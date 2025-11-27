#ifndef LIGHT_SOURCE_H
#define LIGHT_SOURCE_H

#include "Ray.h"

class LightSource {
   public:
    LightSource() = default;
    LightSource(const LightSource&) = default;
    LightSource(LightSource&&) = delete;
    LightSource& operator=(const LightSource&) = default;
    LightSource& operator=(LightSource&&) = delete;
    virtual ~LightSource() = default;

    /**
     * Get the intensity of the Light.
     *
     * @return Intensity as an Eigen::Vector3f.
     */
    [[nodiscard]] virtual const Eigen::Vector3f& intensity() const {
        throw std::runtime_error("intensity() not implemented for this light.");
    }

    /**
     * Given a query point return a ray _toward_ the Light.
     *
     * @param [in] point 3D query point in space
     * @return Ray from `point` toward the Light
     */
    [[nodiscard]] virtual Ray ray_from(Eigen::Vector3f point) const = 0;

    /**
     * Compute the angular size of this light from a given point.
     *
     * @param [in] ray Ray from the point of view.
     * @param [in] distance Distance from the point of view to the light.
     * @return Angular size in radians.
     */
    [[nodiscard]] virtual float angular_size_from(const Ray& ray,
                                                  float distance) const = 0;
};

#endif
