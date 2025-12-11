#ifndef OBJECT_H
#define OBJECT_H

#include <Eigen/Core>

#include "Ray.h"

class Object {
   public:
    Object() = default;

    Object(const Object&) = default;
    Object(Object&&) = delete;
    Object& operator=(const Object&) = default;
    Object& operator=(Object&&) = delete;
    virtual ~Object() = default;

    /**
     * Generate a ray from a given point toward the Light object.
     *
     * @param [in] point Point from which the ray is generated.
     * @return Ray from `point` toward the Light.
     */
    [[nodiscard]] virtual Ray ray_from(
        [[maybe_unused]] const Eigen::Vector3f point) const {
        throw std::logic_error("ray_from() not implemented for this object.");
    }

    /**
     * Compute the multiplication inverse of the probability density function
     * (PDF) value for sampling a direction toward the object from all
     * directions to the object.
     *
     * @param [in] ray Ray from the point of view.
     * @param [in] distance Distance to the sampled point on the object.
     * @return PDF value.
     */
    [[nodiscard]] virtual float inv_pdf(
        [[maybe_unused]] const Ray& ray,
        [[maybe_unused]] const float distance) const {
        throw std::logic_error("inv_pdf() not implemented for this object.");
    }

    /**
     * Get the emission of the object.
     *
     * @param [in] texcoords Texture coordinates.
     * @return Emission.
     */
    [[nodiscard]] virtual Eigen::Vector3f emission_at(
        [[maybe_unused]] const Eigen::Vector2f& texcoords) const {
        throw std::logic_error(
            "emission_at() not implemented for this object.");
    }
};

#endif
