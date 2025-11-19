#include "Sphere.h"

#include "Ray.h"

static bool calc_t(const Ray& ray, const Sphere& sphere, const double min_t,
                   double& t) {
    const double a = ray.direction.squaredNorm();
    const double b = 2 * ray.direction.dot(ray.origin - sphere.center);
    const double c = (ray.origin - sphere.center).squaredNorm() -
                     sphere.radius * sphere.radius;
    const double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) return false;

    // smaller t
    t = (-b - sqrt(discriminant)) / (2 * a);
    if (t >= min_t) return true;

    // larger t
    t = (-b + sqrt(discriminant)) / (2 * a);
    if (t >= min_t) return true;

    return false;
}

bool Sphere::intersect(const Ray& ray, const double min_t, double& out_t,
                       Eigen::Vector3f& out_n) const {
    double t;
    if (!calc_t(ray, *this, min_t, t)) return false;

    out_t = t;
    out_n = (ray.origin + t * ray.direction - this->center).normalized();
    return true;
}
