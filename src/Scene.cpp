#include "Scene.h"

#include <iostream>

#include "bvh/build_bvh.h"

Scene::Scene(Camera camera, std::vector<std::unique_ptr<Geometry>> geometries,
             std::vector<std::unique_ptr<Light>> lights)
    : camera(std::move(camera)), lights(std::move(lights)) {
    std::cout << "Geometries: " << geometries.size() << "\n";
    std::cout << "Lights: " << this->lights.size() << "\n";

    // Collect emissive objects
    emissive_objects.reserve(this->lights.size());
    for (const auto& light : this->lights) {
        emissive_objects.emplace_back(light.get());
    }
    for (const auto& geometry : geometries) {
        if (geometry->material &&
            geometry->material->emission.maxCoeff() > 0.F) {
            emissive_objects.emplace_back(geometry.get());
        }
    }
    emissive_objects.shrink_to_fit();

    std::cout << "Emissive objects: " << emissive_objects.size() << "\n";

    this->geometries = build_bvh(std::move(geometries));
}
