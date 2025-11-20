#ifndef READ_JSON_H
#define READ_JSON_H

#include <string>

#include "Scene.h"

/**
 * Read a scene description from a .json file
 *
 * @param filename path to .json file
 * @return Scene object representing the scene
 */
Scene read_json(const std::string& filename);

#endif
