/**
 * Cook-Torrance BRDF.
 *
 * Reference:
 * https://learnopengl.com/PBR/Theory
 */

#include "brdf.h"

#include <Eigen/Core>
#include <numbers>

#include "object/Object.h"

namespace {

static const float PI = std::numbers::pi_v<float>;
static const float EPSILON = 1e-6F;

template <typename T>
static T mix(const T& x, const T& y, const float a) {
    return x * (1 - a) + y * a;
}

static float pow2(const float x) { return x * x; }

static float pow5(const float x) {
    const float x2 = x * x;
    return x2 * x2 * x;
}

}  // namespace

Eigen::Vector3f brdf(const Ray& view_to_surface, const Ray& surface_to_light,
                     const Intersection& view_intersection,
                     const Eigen::Vector3f& normal) {
    const auto& material = view_intersection.object->material;

    const Eigen::Vector3f h =
        (-view_to_surface.direction + surface_to_light.direction).normalized();
    const float n_dot_h = normal.dot(h);
    const float n_dot_l = normal.dot(surface_to_light.direction);
    const float n_dot_v = normal.dot(-view_to_surface.direction);
    const float h_dot_v = h.dot(-view_to_surface.direction);

    // Lambertian diffuse
    const auto diffuse = material->diffuse / PI;

    // Cook-Torrance specular

    // Normal Distribution Function (Trowbridge-Reitz GGX)
    const float a2 = pow2(material->roughness);
    const float specular_d =
        a2 / (PI * pow2(pow2(n_dot_h) * (a2 - 1) + 1) + EPSILON);

    // Geometry Function (Smith's method with Schlick-GGX)
    const float k = pow2(material->roughness + 1) / 8;
    const float g_sub_1 = n_dot_v / (n_dot_v * (1 - k) + k);
    const float g_sub_2 = n_dot_l / (n_dot_l * (1 - k) + k);
    const float specular_g = g_sub_1 * g_sub_2;

    // Fresnel Equation (Fresnel-Schlick approximation)
    const auto f0 = mix<Eigen::Vector3f>(Eigen::Vector3f::Constant(0.04F),
                                         material->diffuse, material->metallic);
    const auto specular_f =
        f0 + (Eigen::Vector3f::Ones() - f0) * pow5(1 - h_dot_v);

    const auto specular =
        (specular_d * specular_g * specular_f) / (4 * n_dot_l * n_dot_v);

    return diffuse + specular;
}
