#include "build_bvh.h"

#include "AABBTree.h"

std::unique_ptr<Geometry> build_bvh(
    std::vector<std::unique_ptr<Geometry>> objects) {
    if (objects.empty()) return std::make_unique<Geometry>();
    if (objects.size() == 1) return std::move(objects[0]);
    return std::make_unique<AABBTree>(std::move(objects));
}
