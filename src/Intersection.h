#ifndef INTERSECION_H
#define INTERSECION_H

#include <Eigen/Core>
#include <limits>

class Object;

class Intersection {
   public:
    Intersection()
        : object(nullptr),
          t(std::numeric_limits<float>::infinity()),
          barycentric_coords(Eigen::Vector3f::Zero()) {}

    Intersection(const Object* const object, const float t,
                 Eigen::Vector3f barycentric_coords = Eigen::Vector3f::Zero())
        : object(object),
          t(t),
          barycentric_coords(std::move(barycentric_coords)) {}

    [[nodiscard]] const Intersection& earlier(const Intersection& other) const {
        return (t < other.t) ? *this : other;
    }

    [[nodiscard]] bool has_intersection() const { return object != nullptr; }

    static const Intersection NoIntersection() { return {}; }

    // Pointer to the intersected object (nullptr if no intersection)
    const Object* object;
    // Parametric distance along the ray to the intersection point
    float t;
    // Barycentric coordinates at the intersection point
    Eigen::Vector3f barycentric_coords;
};

#endif
