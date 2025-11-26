#include "Triangle.h"

#include <Eigen/Dense>

#include "../Ray.h"
#include "../barycentric_interpolation.h"

Triangle::Triangle(
    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> corners,
    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> normals,
    std::tuple<Eigen::Vector2f, Eigen::Vector2f, Eigen::Vector2f> texcoords,
    std::shared_ptr<Material> material)
    : Object(std::move(material), AABB(get<0>(corners)
                                           .cwiseMin(get<1>(corners))
                                           .cwiseMin(get<2>(corners)),
                                       get<0>(corners)
                                           .cwiseMax(get<1>(corners))
                                           .cwiseMax(get<2>(corners)))),
      corners(std::move(corners)),
      normals(std::move(normals)),
      texcoords(std::move(texcoords)) {}

Triangle::Triangle(
    std::tuple<Eigen::Vector3f, Eigen::Vector3f, Eigen::Vector3f> corners,
    std::shared_ptr<Material> material)
    : Triangle(std::move(corners), {}, {}, std::move(material)) {
    const auto normal =
        ((std::get<1>(this->corners) - std::get<0>(this->corners))
             .cross(std::get<2>(this->corners) - std::get<0>(this->corners)))
            .normalized();
    this->normals = std::make_tuple(normal, normal, normal);
}

Intersection Triangle::intersect(const Ray& ray) const {
    if (!bounding_box.intersect(ray)) return Intersection::NoIntersection();

    const auto& x1 = std::get<0>(this->corners);
    const auto t1 = std::get<1>(this->corners) - std::get<0>(this->corners);
    const auto t2 = std::get<2>(this->corners) - std::get<0>(this->corners);

    Eigen::Matrix3f a;
    a << t1, t2, -ray.direction;
    const auto b = ray.origin - x1;  // auto for lazy evaluation

    const Eigen::Vector3f x = a.partialPivLu().solve(b);
    const float alpha = x(0);
    const float beta = x(1);
    const float t = x(2);

    if (t < ray.min_t || t > ray.max_t) return Intersection::NoIntersection();
    if (alpha < 0 || beta < 0) return Intersection::NoIntersection();
    if (alpha + beta > 1) return Intersection::NoIntersection();

    const float gamma = 1.F - alpha - beta;
    const Eigen::Vector3f barycentric_coords =
        Eigen::Vector3f(gamma, alpha, beta);

    return {this, t, barycentric_coords};
}

Eigen::Vector3f Triangle::normal_at([[maybe_unused]] const Ray& ray,
                                    const Intersection& intersection) const {
    return barycentric_interpolation(normals, intersection.barycentric_coords);
}

Eigen::Vector2f Triangle::texcoord_at([[maybe_unused]] const Ray& ray,
                                      const Intersection& intersection) const {
    return barycentric_interpolation(texcoords,
                                     intersection.barycentric_coords);
}
