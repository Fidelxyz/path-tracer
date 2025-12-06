/**
 * Cook-Torrance BRDF.
 *
 * Reference:
 * https://learnopengl.com/PBR/Theory
 */

#include "brdf.h"

#include <Eigen/Dense>
#include <numbers>
#include <random>

#include "geometry/Geometry.h"
#include "util/random.h"

namespace {

static const float PI = std::numbers::pi_v<float>;
static const float EPSILON = 1e-6F;

template <typename T>
static constexpr T mix(const T& x, const T& y, const float a) {
    return x * (1 - a) + y * a;
}

static constexpr float pow2(const float x) { return x * x; }

static constexpr float pow5(const float x) {
    const float x2 = x * x;
    return x2 * x2 * x;
}

/**
 * Transform a vector from tangent space to world space given the normal.
 */
static Eigen::Vector3f from_tangent_space(const Eigen::Vector3f& vector,
                                          const Eigen::Vector3f& normal) {
    // https://learnopengl.com/Advanced-Lighting/Normal-Mapping

    // Choose an arbitrary vector that is not parallel to the normal
    const Eigen::Vector3f up = std::abs(normal.z()) < 0.99F
                                   ? Eigen::Vector3f::UnitZ()
                                   : Eigen::Vector3f::UnitY();
    const Eigen::Vector3f tangent = normal.cross(up).normalized();
    const Eigen::Vector3f bitangent = normal.cross(tangent).normalized();
    Eigen::Matrix3f tbn;
    tbn << tangent, bitangent, normal;
    return tbn * vector;
}

static Eigen::Vector3f spherical_to_cartesian(const float theta,
                                              const float phi) {
    return {
        std::sin(theta) * std::cos(phi),
        std::sin(theta) * std::sin(phi),
        std::cos(theta),
    };
}

static std::tuple<float, float> weight_diffuse_specular(
    const Eigen::Vector3f& diffuse, const float metallic, const float n_dot_v) {
    // Weighted by Fresnel term at normal incidence.
    // https://computergraphics.stackexchange.com/questions/13862/how-to-approach-implementing-cook-torrence-microfacet-brdf
    const float albedo =
        diffuse.dot(Eigen::Vector3f(0.2126F, 0.7152F, 0.0722F));
    const float f0 = mix(0.04F, albedo, metallic);
    const float fresnel = f0 + (1 - f0) * pow5(1 - n_dot_v);
    return {1 - fresnel, fresnel};
}

}  // namespace

Eigen::Vector3f brdf(const Ray& view_to_surface, const Ray& surface_to_light,
                     const Intersection& view_intersection,
                     const Eigen::Vector3f& normal,
                     const Eigen::Vector2f& texcoords) {
    const auto& material = view_intersection.object->material;

    const Eigen::Vector3f& mat_diffuse = material->diffuse->sample(texcoords);
    const float mat_roughness = material->roughness->sample(texcoords);
    const float mat_metallic = material->metallic->sample(texcoords);

    const Eigen::Vector3f h =
        (-view_to_surface.direction + surface_to_light.direction).normalized();
    const float n_dot_h = std::max(normal.dot(h), 0.F);
    const float n_dot_l = normal.dot(surface_to_light.direction);
    const float n_dot_v = std::max(normal.dot(-view_to_surface.direction), 0.F);
    const float h_dot_v = h.dot(-view_to_surface.direction);

    // Lambertian diffuse
    const auto diffuse = mat_diffuse / PI;

    // Cook-Torrance specular

    // Normal Distribution Function (Trowbridge-Reitz GGX)
    const float a = pow2(mat_roughness);  // a = roughness^2
    const float a2 = pow2(a);
    const float specular_d =
        a2 / (PI * pow2(pow2(n_dot_h) * (a2 - 1) + 1) + EPSILON);

    // Geometry Function (Smith's method with Schlick-GGX)
    const float k = pow2(a + 1) / 8;
    const float g_sub_1 = n_dot_v / (n_dot_v * (1 - k) + k);
    const float g_sub_2 = n_dot_l / (n_dot_l * (1 - k) + k);
    const float specular_g = g_sub_1 * g_sub_2;

    // Fresnel Equation (Fresnel-Schlick approximation)
    const auto f0 = mix<Eigen::Vector3f>(Eigen::Vector3f::Constant(0.04F),
                                         mat_diffuse, mat_metallic);
    const auto specular_f =
        f0 + (Eigen::Vector3f::Ones() - f0) * pow5(1 - h_dot_v);

    const auto specular = (specular_d * specular_g * specular_f) /
                          (4 * n_dot_l * n_dot_v + EPSILON);

    return diffuse + specular;
}

Ray brdf_sample(const Ray& view_to_surface,
                const Intersection& view_intersection,
                const Eigen::Vector3f& surface_point,
                const Eigen::Vector3f& normal,
                const Eigen::Vector2f& texcoords) {
    const auto& material = view_intersection.object->material;

    const auto sample_diffuse = [&]() -> Eigen::Vector3f {
        // Cosine-weighted hemisphere sampling
        // https://ameye.dev/notes/sampling-the-hemisphere/
        std::uniform_real_distribution<float> uniform_dist(0.F, 1.F);
        const float r1 = uniform_dist(rng);
        const float r2 = uniform_dist(rng);

        const float theta = std::acos(std::sqrt(1 - r1));
        const float phi = 2 * PI * r2;

        return from_tangent_space(spherical_to_cartesian(theta, phi), normal);
    };

    const auto sample_specular = [&]() -> Eigen::Vector3f {
        const float roughness = material->roughness->sample(texcoords);

        // Importance sampling for GGX distribution
        // https://agraphicsguynotes.com/posts/sample_microfacet_brdf/
        std::uniform_real_distribution<float> uniform_dist(0.F, 1.F);
        const float r1 = uniform_dist(rng);
        const float r2 = uniform_dist(rng);

        const float a = pow2(roughness);  // a = roughness^2
        const float theta = std::atan(a * std::sqrt(r1 / (1 - r1)));
        const float phi = 2 * PI * r2;

        const auto h =
            from_tangent_space(spherical_to_cartesian(theta, phi), normal);

        const auto v = -view_to_surface.direction;
        const auto direction = (2 * h.dot(v) * h - v).normalized();
        return direction;
    };

    const Eigen::Vector3f& diffuse = material->diffuse->sample(texcoords);
    const float metallic = material->metallic->sample(texcoords);
    const auto [weight_diffuse, weight_specular] = weight_diffuse_specular(
        diffuse, metallic, normal.dot(-view_to_surface.direction));

    Eigen::Vector3f direction;
    std::uniform_real_distribution<float> uniform_dist(0.F, 1.F);
    const float r = uniform_dist(rng);
    if (r < weight_specular) {
        direction = sample_specular();
    } else {
        direction = sample_diffuse();
    }

    return {surface_point, direction};
}

float brdf_pdf(const Ray& view_to_surface, const Ray& surface_to_light,
               const Intersection& view_intersection,
               const Eigen::Vector3f& normal,
               const Eigen::Vector2f& texcoords) {
    const auto& material = view_intersection.object->material;
    const Eigen::Vector3f& diffuse = material->diffuse->sample(texcoords);
    const float roughness = material->roughness->sample(texcoords);
    const float metallic = material->metallic->sample(texcoords);

    const Eigen::Vector3f h =
        (-view_to_surface.direction + surface_to_light.direction).normalized();
    const float n_dot_h = std::max(normal.dot(h), 0.F);
    const float n_dot_l = normal.dot(surface_to_light.direction);
    const float n_dot_v = std::max(normal.dot(-view_to_surface.direction), 0.F);
    const float h_dot_l = h.dot(surface_to_light.direction);

    // PDF for diffuse component (cosine-weighted hemisphere)
    const float pdf_diffuse = n_dot_l / PI;

    // PDF for specular component (GGX)
    const float a = pow2(roughness);  // a = roughness^2
    const float a2 = pow2(a);
    const float pdf_specular = (a2 * n_dot_h) /
                               (PI * pow2(pow2(n_dot_h) * (a2 - 1) + 1)) /
                               (4 * h_dot_l);

    const auto [weight_diffuse, weight_specular] =
        weight_diffuse_specular(diffuse, metallic, n_dot_v);

    return pdf_diffuse * weight_diffuse + pdf_specular * weight_specular;
}
