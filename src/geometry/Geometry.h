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
     * @param ray Ray to intersect with.
     * @return Intersection information.
     */
    [[nodiscard]] virtual Intersection intersect(
        [[maybe_unused]] const Ray& ray) const {
        return Intersection::NoIntersection();
    }

    /**
     * Get the geometry normal at a given point on the object's surface.
     *
     * @param ray Incoming ray.
     * @param point Point on the object's surface.
     * @return Normal vector.
     */
    [[nodiscard]] virtual Eigen::Vector3f normal_at(
        [[maybe_unused]] const Ray& ray,
        [[maybe_unused]] const Eigen::Vector3f& point) const {
        throw std::runtime_error(
            "normal_at() not implemented for this object.");
    }

    /**
     * Compute the tangent space matrix at a given point on the object's
     * surface.
     *
     * @param point Point on the object's surface.
     * @param normal Normal vector at the given point.
     * @return Tangent space matrix where columns are tangent, bitangent,
     *         and normal vectors.
     */
    [[nodiscard]] virtual Eigen::Matrix3f tangent_space_at(
        [[maybe_unused]] const Eigen::Vector3f& point,
        [[maybe_unused]] const Eigen::Vector3f& normal) const {
        throw std::runtime_error(
            "tangent_space_at() not implemented for this object.");
    }

    /**
     * Get texture coordinates at a given point on the object's surface.
     *
     * @param point Point on the object's surface.
     * @return Texture coordinates at the given point.
     */
    [[nodiscard]] virtual Eigen::Vector2f texcoords_at(
        [[maybe_unused]] const Eigen::Vector3f& point) const {
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
