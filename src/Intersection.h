#ifndef INTERSECION_H
#define INTERSECION_H

#include <Eigen/Core>
#include <limits>

class Geometry;

class Intersection {
   public:
    constexpr Intersection()
        : object(nullptr), t(std::numeric_limits<float>::infinity()) {}

    Intersection(const Geometry* const object, const float t)
        : object(object), t(t) {}

    [[nodiscard]] const Intersection& earlier(const Intersection& other) const {
        return (t < other.t) ? *this : other;
    }

    [[nodiscard]] bool has_intersection() const { return object != nullptr; }

    static constexpr Intersection NoIntersection() { return {}; }

    // Pointer to the intersected object (nullptr if no intersection).
    const Geometry* object;
    // Distance along the ray to the intersection point.
    float t;
};

#endif
