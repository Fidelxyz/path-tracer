#include "Object.h"

#include <cassert>

Object::Object(const std::shared_ptr<Material> material,
               const AABB bounding_box)
    : material(std::move(material)), bounding_box(std::move(bounding_box)) {}

Object::~Object() {}

Eigen::Vector3f Object::intersection_normal([[maybe_unused]] const Ray& ray,
                                            [[maybe_unused]] float t) const {
    assert(false && "intersection_normal not implemented for this object");
    return {};
}
