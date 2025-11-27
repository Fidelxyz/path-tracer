#ifndef SCENE_H
#define SCENE_H

#include <cassert>
#include <iostream>

#include "Camera.h"
#include "bvh/AABBTree.h"
#include "light/Light.h"
#include "object/Object.h"

using Objects = std::vector<std::unique_ptr<Object>>;
using Lights = std::vector<std::unique_ptr<Light>>;

class Scene {
   public:
    Scene(Camera camera, Objects objects, Lights lights)
        : camera(std::move(camera)), lights(std::move(lights)) {
        assert(!objects.empty());

        std::cout << "Objects: " << objects.size() << '\n';
        std::cout << "Lights: " << this->lights.size() << '\n';

        // Collect emissive objects
        for (const auto& obj : objects) {
            if (obj->material && !obj->material->emission.isZero()) {
                emissive_objects.emplace_back(obj.get());
            }
        }

        std::cout << "Emissive objects: " << emissive_objects.size() << '\n';

        assert((!this->lights.empty() || !emissive_objects.empty()) &&
               "Scene must have at least one light source or emissive object.");

        // Build BVH
        if (objects.size() == 1) {
            this->objects = std::move(objects[0]);
        } else {
            this->objects = std::make_unique<AABBTree>(std::move(objects));
        }
    }

    Camera camera;
    Lights lights;
    std::unique_ptr<Object> objects;
    std::vector<const Object*> emissive_objects;
};

#endif
