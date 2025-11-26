#ifndef AABBTREE_H
#define AABBTREE_H

#include <Eigen/Core>
#include <memory>

#include "../object/Object.h"

class AABBTree : public Object {
   public:
    /**
     * Constructs an axis-aligned bounding box (AABB) tree from a list of
     * objects. The tree is built by recursively splitting the objects based on
     * the midpoint along the longest axis of their bounding box.
     *
     * @param objects A vector of shared pointers to the objects to be included
     * in the AABB tree.
     * @param depth The depth of the current node in the tree (default is 0
     * for the root node).
     */
    explicit AABBTree(std::vector<std::unique_ptr<Object>> objects);

    [[nodiscard]] Intersection intersect(const Ray& ray) const override;

    std::unique_ptr<Object> left;
    std::unique_ptr<Object> right;
};

#endif
