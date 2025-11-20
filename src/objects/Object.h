#ifndef OBJECT_H
#define OBJECT_H

#include <Eigen/Core>
#include <memory>

#include "../Intersection.h"
#include "../Material.h"
#include "../Ray.h"
#include "../bvh/AABB.h"

class Object {
   public:
    Object() = default;
    Object(std::shared_ptr<Material> material, AABB bounding_box);

    // https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
    virtual ~Object();

    /**
     * Intersect object with ray.
     *
     * @param [in] ray Ray to intersect with.
     * @param [in] min_t Minimum parametric distance to consider.
     * @param [in] max_t Maximum parametric distance to consider.
     * @return Intersection information.
     */
    virtual Intersection intersect(const Ray& ray) const = 0;

    /**
     * Get normal at a given point on the object's surface.
     *
     * @param [in] ray Ray that intersected the object.
     * @param [in] t Parametric distance along the ray to the intersection
     *               point.
     * @return Normal vector at the given point.
     */
    virtual Eigen::Vector3f intersection_normal(const Ray& ray, float t) const;

    const std::shared_ptr<Material> material;
    AABB bounding_box;
};

#endif
