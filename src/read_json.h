#ifndef READ_JSON_H
#define READ_JSON_H

#include <memory>
#include <string>
#include <vector>

#include "Camera.h"
#include "Light.h"
#include "Object.h"

// Read a scene description from a .json file
//
// Input:
//   filename  path to .json file
// Output:
//   camera  camera looking at the scene
//   objects  list of shared pointers to objects
//   lights  list of shared pointers to lights
bool read_json(const std::string& filename, Camera& camera,
               std::vector<std::shared_ptr<Object>>& objects,
               std::vector<std::shared_ptr<Light>>& lights);

#endif
