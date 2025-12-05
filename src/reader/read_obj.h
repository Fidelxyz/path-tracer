#ifndef READ_OBJ_H
#define READ_OBJ_H

#include <filesystem>
#include <memory>
#include <vector>

#include "../geometry/Geometry.h"

std::vector<std::unique_ptr<Geometry>> read_obj(
    const std::filesystem::path& obj_file);

#endif
