#ifndef MATERIAL_H
#define MATERIAL_H

#include <Eigen/Core>

struct Material {
    Eigen::Vector3f ambient;
    Eigen::Vector3f diffuse;
    Eigen::Vector3f specular;
    Eigen::Vector3f transmittance;
    Eigen::Vector3f emission;
    float shininess;
    float ior;

    // PBR extension
    float roughness;
    float metallic;
    float sheen;
    float clearcoat_thickness;
    float clearcoat_roughness;
    float anisotropy;
    float anisotropy_rotation;
};

#endif
