#ifndef INTERSECION_H
#define INTERSECION_H

#include <limits>

class Object;

class Intersection {
   public:
    Intersection()
        : object(nullptr), t(std::numeric_limits<float>::infinity()) {}

    Intersection(const Object* const object, const float t)
        : object(object), t(t) {}

    [[nodiscard]] Intersection earlier(Intersection other) const {
        return (t < other.t) ? *this : other;
    }

    [[nodiscard]] bool has_intersection() const { return object != nullptr; }

    static Intersection NoIntersection() { return {}; }

    // Pointer to the intersected object (nullptr if no intersection)
    const Object* object;
    // Parametric distance along the ray to the intersection point
    float t;
};

#endif
