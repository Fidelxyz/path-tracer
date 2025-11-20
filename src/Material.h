#ifndef MATERIAL_H
#define MATERIAL_H

#include <Eigen/Core>

// Blinn-Phong Approximate Shading Material Parameters
class Material {
   public:
    Material(Eigen::Vector3f ambient, Eigen::Vector3f diffuse,
             Eigen::Vector3f specular, Eigen::Vector3f mirror, float phong_exp)
        : ka(std::move(ambient)),
          kd(std::move(diffuse)),
          ks(std::move(specular)),
          km(std::move(mirror)),
          phong_exponent(phong_exp) {}

    // Ambient, Diffuse, Specular, Mirror Color
    Eigen::Vector3f ka, kd, ks, km;
    // Phong exponent
    float phong_exponent;
};
#endif
