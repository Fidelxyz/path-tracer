#ifndef INTERSECION_H
#define INTERSECION_H

#include <limits>
#include <utility>

class Object;

class Intersection {
   public:
    Intersection()
        : object(nullptr), t(std::numeric_limits<float>::infinity()) {}

    Intersection(const Object* const object, const float t)
        : object(object), t(t) {}

    const Intersection&& earlier(const Intersection&& other) const {
        return (t < other.t) ? std::forward<const Intersection>(*this)
                             : std::forward<const Intersection>(other);
    }

    bool has_intersection() const { return object != nullptr; }

    static inline Intersection NoIntersection() { return Intersection(); }

    // Pointer to the intersected object (nullptr if no intersection)
    const Object* const object;
    // Parametric distance along the ray to the intersection point
    const float t;
};

#endif
