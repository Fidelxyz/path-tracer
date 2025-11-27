#ifndef OBJECT_H
#define OBJECT_H

#include <Eigen/Core>
#include <memory>

#include "../Intersection.h"
#include "../LightSource.h"
#include "../Material.h"
#include "../Ray.h"
#include "../bvh/AABB.h"

class Object : public LightSource {
   public:
    Object() = default;
    Object(std::shared_ptr<Material> material, AABB bounding_box)
        : material(std::move(material)),
          bounding_box(std::move(bounding_box)) {}

    /**
     * Intersect object with ray.
     *
     * @param [in] ray Ray to intersect with.
     * @return Intersection information.
     */
    [[nodiscard]] virtual Intersection intersect(const Ray& ray) const = 0;

    /**
     * Get normal at a given point on the object's surface.
     *
     * @param [in] ray Incoming ray.
     * @param [in] intersecetion Intersection information.
     * @return Normal vector at the given point.
     */
    [[nodiscard]] virtual Eigen::Vector3f normal_at(
        [[maybe_unused]] const Ray& ray,
        [[maybe_unused]] const Intersection& intersecetion) const {
        throw std::runtime_error(
            "normal_at() not implemented for this object.");
    }

    /**
     * Generate a ray from a given point towards an arbitrary point on this
     * object.
     *
     * @param [in] point Point from which the ray is generated.
     * @return Generated ray.
     */
    [[nodiscard]] Ray ray_from(
        [[maybe_unused]] Eigen::Vector3f point) const override {
        throw std::runtime_error("ray_from() not implemented for this object.");
    }

    /**
     * Compute the angular size of this object from a given point.
     *
     * @param [in] ray Ray from the point of view.
     * @param [in] distance Distance from the point of view to the object.
     * @return Angular size in radians.
     */
    [[nodiscard]] float angular_size_from(
        [[maybe_unused]] const Ray& ray,
        [[maybe_unused]] const float distance) const override {
        throw std::runtime_error(
            "angular_size_from() not implemented for this object.");
    }

    std::shared_ptr<Material> material;
    AABB bounding_box;
};

#endif
