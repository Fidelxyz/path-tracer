#ifndef AABB_H
#define AABB_H

#include <Eigen/Core>

#include "../Ray.h"

class AABB {
   public:
    /**
     * Construct an empty Axis Aligned Bounding Box.
     */
    AABB();

    /**
     * Construct a new Axis Aligned Bounding Box.
     *
     * @param min_corner The minimum corner of the AABB.
     * @param max_corner The maximum corner of the AABB.
     */
    AABB(Eigen::Vector3f min_corner, Eigen::Vector3f max_corner);

    void merge(const AABB& other);

    [[nodiscard]] Eigen::Vector3f center() const;
    [[nodiscard]] Eigen::Vector3f dimensions() const;
    [[nodiscard]] bool intersect(const Ray& ray) const;

    Eigen::Vector3f min_corner;
    Eigen::Vector3f max_corner;
};

#endif
