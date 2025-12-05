#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "../Intersection.h"
#include "../Object.h"
#include "../bvh/AABB.h"
#include "../material/Material.h"

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
    [[nodiscard]] virtual Intersection intersect(const Ray& /*ray*/) const {
        return Intersection::NoIntersection();
    }

    /**
     * Get normal at a given point on the object's surface.
     *
     * @param [in] ray Incoming ray.
     * @param [in] point Point on the object's surface.
     * @return Normal vector at the given point.
     */
    [[nodiscard]] virtual Eigen::Vector3f normal_at(
        const Ray& /*ray*/, const Eigen::Vector3f& /*point*/) const {
        throw std::runtime_error(
            "normal_at() not implemented for this object.");
    }

    /**
     * Get texture coordinates at a given point on the object's surface.
     *
     * @param [in] point Point on the object's surface.
     * @return Texture coordinates at the given point.
     */
    [[nodiscard]] virtual Eigen::Vector2f texcoords_at(
        const Eigen::Vector3f& /*point*/) const {
        throw std::runtime_error(
            "texcoords_at() not implemented for this object.");
    }

    [[nodiscard]] Eigen::Vector3f emission_at(
        const Eigen::Vector2f& texcoords) const override {
        return material->emission->sample(texcoords);
    }

    AABB bounding_box;
    std::shared_ptr<Material> material;
};

#endif
