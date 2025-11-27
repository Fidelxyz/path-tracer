#ifndef MATERIAL_H
#define MATERIAL_H

#include <Eigen/Core>

class Material {
   public:
    Material(Eigen::Vector3f ambient, Eigen::Vector3f diffuse,
             Eigen::Vector3f specular, Eigen::Vector3f transmittance,
             Eigen::Vector3f emission, const float shininess, const float ior)
        : ambient(std::move(ambient)),
          diffuse(std::move(diffuse)),
          specular(std::move(specular)),
          transmittance(std::move(transmittance)),
          emission(std::move(emission)),
          shininess(shininess),
          ior(ior) {}

    Eigen::Vector3f ambient;
    Eigen::Vector3f diffuse;
    Eigen::Vector3f specular;
    Eigen::Vector3f transmittance;
    Eigen::Vector3f emission;
    float shininess;
    float ior;
    int illum;
};

#endif
