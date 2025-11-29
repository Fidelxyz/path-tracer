#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "../Intersection.h"
#include "../Material.h"
#include "../Object.h"
#include "../bvh/AABB.h"

class Geometry : public Object {
   public:
    explicit Geometry(AABB bounding_box = {},
                      std::shared_ptr<Material> material = nullptr)
        : bounding_box(std::move(bounding_box)),
          material(std::move(material)) {}

    /**
     * Intersect object with ray.
     *
     * @param [in] ray Ray to intersect with.
     * @return Intersection information.
     */
    [[nodiscard]] virtual Intersection intersect(
        [[maybe_unused]] const Ray& ray) const {
        return Intersection::NoIntersection();
    }

    /**
     * Get normal at a given point on the object's surface.
     *
     * @param [in] ray Incoming ray.
     * @param [in] intersection Intersection information.
     * @return Normal vector at the given point.
     */
    [[nodiscard]] virtual Eigen::Vector3f normal_at(
        [[maybe_unused]] const Ray& ray,
        [[maybe_unused]] const Intersection& intersection) const {
        throw std::runtime_error(
            "normal_at() not implemented for this object.");
    }

    [[nodiscard]] const Eigen::Vector3f& emission() const override {
        return material->emission;
    }

    AABB bounding_box;
    std::shared_ptr<Material> material;
};

#endif
