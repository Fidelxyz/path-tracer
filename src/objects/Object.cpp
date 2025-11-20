#include "Object.h"

#include <cassert>

Object::Object(std::shared_ptr<Material> material, AABB bounding_box)
    : material(std::move(material)), bounding_box(std::move(bounding_box)) {}

Eigen::Vector3f Object::intersection_normal([[maybe_unused]] const Ray& ray,
                                            [[maybe_unused]] float t) const {
    assert(false && "intersection_normal not implemented for this object");
    return {};
}
