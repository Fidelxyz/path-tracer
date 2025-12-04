#ifndef SCENE_H
#define SCENE_H

#include <cassert>

#include "Camera.h"
#include "Object.h"
#include "Options.h"
#include "geometry/Geometry.h"
#include "light/Light.h"

class Scene {
   public:
    Scene(Options options, Camera camera,
          std::vector<std::unique_ptr<Geometry>> geometries,
          std::vector<std::unique_ptr<Light>> lights);

    Options options;
    Camera camera;
    // The BVH for all geometries
    std::unique_ptr<Geometry> geometries;
    // Lights
    std::vector<std::unique_ptr<Light>> lights;
    // List of emissive objects for random access
    std::vector<Object*> emissive_objects;
};

#endif
