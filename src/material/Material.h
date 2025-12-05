#ifndef MATERIAL_H
#define MATERIAL_H

#include <Eigen/Core>
#include <memory>

#include "Sampler.h"

struct Material {
    // Eigen::Vector3f ambient;
    std::unique_ptr<SamplerRGB> diffuse;
    // Eigen::Vector3f specular;
    // Eigen::Vector3f transmittance;
    std::unique_ptr<SamplerRGB> emission;
    // float shininess;
    // float ior;

    // PBR extension
    std::unique_ptr<SamplerGray> roughness;
    std::unique_ptr<SamplerGray> metallic;
    // float sheen;
    // float clearcoat_thickness;
    // float clearcoat_roughness;
    // float anisotropy;
    // float anisotropy_rotation;

    // Precomputed data
    bool emissive;
};

#endif
