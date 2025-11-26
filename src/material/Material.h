#ifndef MATERIAL_H
#define MATERIAL_H

#include <Eigen/Core>

#include "Sampler.h"

class Material {
   public:
    Material(std::unique_ptr<SamplerRGB> ambient,
             std::unique_ptr<SamplerRGB> diffuse,
             std::unique_ptr<SamplerRGB> specular,
             std::unique_ptr<SamplerRGB> transmittance,
             std::unique_ptr<SamplerRGB> emission,
             std::unique_ptr<SamplerGray> shininess,
             std::unique_ptr<SamplerGray> ior,
             std::unique_ptr<SamplerGray> roughness,
             std::unique_ptr<SamplerGray> metallic,
             std::unique_ptr<SamplerGray> sheen)
        : ambient(std::move(ambient)),
          diffuse(std::move(diffuse)),
          specular(std::move(specular)),
          transmittance(std::move(transmittance)),
          emission(std::move(emission)),
          shininess(std::move(shininess)),
          ior(std::move(ior)),
          roughness(std::move(roughness)),
          metallic(std::move(metallic)),
          sheen(std::move(sheen)) {}

    std::unique_ptr<SamplerRGB> ambient;
    std::unique_ptr<SamplerRGB> diffuse;
    std::unique_ptr<SamplerRGB> specular;
    std::unique_ptr<SamplerRGB> transmittance;
    std::unique_ptr<SamplerRGB> emission;
    std::unique_ptr<SamplerGray> shininess;
    std::unique_ptr<SamplerGray> ior;
    std::unique_ptr<SamplerGray> roughness;
    std::unique_ptr<SamplerGray> metallic;
    std::unique_ptr<SamplerGray> sheen;
};

#endif
