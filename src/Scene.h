#ifndef SCENE_H
#define SCENE_H

#include "Camera.h"
#include "bvh/AABBTree.h"
#include "lights/Light.h"
#include "objects/Object.h"

using Objects = std::vector<std::unique_ptr<Object>>;
using Lights = std::vector<std::unique_ptr<Light>>;

class Scene {
   public:
    Scene(Camera camera, Objects objects, Lights lights)
        : camera(std::move(camera)),
          objects([](Objects objects) -> std::unique_ptr<Object> {
              assert(!objects.empty());
              if (objects.size() == 1) return std::move(objects[0]);
              return std::make_unique<AABBTree>(std::move(objects));
          }(std::move(objects))),
          lights(std::move(lights)) {}

    const Camera camera;
    const std::unique_ptr<Object> objects;
    const Lights lights;
};

#endif
