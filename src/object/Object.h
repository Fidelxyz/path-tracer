#ifndef OBJECT_H
#define OBJECT_H

#include <Eigen/Core>
#include <memory>

#include "../Intersection.h"
#include "../Ray.h"
#include "../bvh/AABB.h"
#include "../material/Material.h"

class Object {
   public:
    Object() = default;
    Object(std::shared_ptr<Material> material, AABB bounding_box)
        : material(std::move(material)),
          bounding_box(std::move(bounding_box)) {}

    // https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
    virtual ~Object() = default;

    /**
     * Intersect object with ray.
     *
     * @param [in] ray Ray to intersect with.
     * @param [in] min_t Minimum parametric distance to consider.
     * @param [in] max_t Maximum parametric distance to consider.
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
     * Get texture coordinates at a given point on the object's surface.
     *
     * @param [in] ray Incoming ray.
     * @param [in] intersecetion Intersection information.
     * @return Texture coordinates at the given point.
     */
    [[nodiscard]] virtual Eigen::Vector2f texcoord_at(
        [[maybe_unused]] const Ray& ray,
        [[maybe_unused]] const Intersection& intersecetion) const {
        throw std::runtime_error(
            "texcoord_at() not implemented for this object.");
    }

    std::shared_ptr<Material> material;
    AABB bounding_box;
};

#endif
