#ifndef MATERIAL_H
#define MATERIAL_H

#include <Eigen/Core>
#include <memory>

#include "Sampler.h"
#include "Texture.h"

struct Material {
    std::unique_ptr<SamplerRGB> diffuse;
    std::unique_ptr<SamplerRGB> emission;
    std::unique_ptr<SamplerGray> roughness;
    std::unique_ptr<SamplerGray> metallic;
    std::unique_ptr<TextureRGB> normal;

    // Precomputed data
    bool emissive;
};

#endif
