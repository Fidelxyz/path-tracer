#ifndef BUILD_BVH_H
#define BUILD_BVH_H

#include "../geometry/Geometry.h"

std::unique_ptr<Geometry> build_bvh(
    std::vector<std::unique_ptr<Geometry>> objects);

#endif
