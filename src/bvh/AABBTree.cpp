#include "AABBTree.h"

#include <algorithm>
#include <cassert>
#include <memory>

AABBTree::AABBTree(std::vector<std::unique_ptr<Geometry>> objects) {
    assert(objects.size() >= 2);

    for (const auto& obj : objects) {
        bounding_box.merge(obj->bounding_box);
    }
    const Eigen::Vector3f dimensions = bounding_box.dimensions();

    int longest_axis = 0;
    if (dimensions(1) > dimensions(longest_axis)) longest_axis = 1;
    if (dimensions(2) > dimensions(longest_axis)) longest_axis = 2;

    const float mid = (bounding_box.min_corner(longest_axis) +
                       bounding_box.max_corner(longest_axis)) /
                      2;

    std::vector<std::unique_ptr<Geometry>> objs_left;
    std::vector<std::unique_ptr<Geometry>> objs_right;

    for (auto& obj : objects) {
        if (obj->bounding_box.center()(longest_axis) < mid) {
            objs_left.push_back(std::move(obj));
        } else {
            objs_right.push_back(std::move(obj));
        }
    }

    if (objs_left.size() == 0 || objs_right.size() == 0) {
        // degenerate to even split if one side is empty

        const auto compare_center = [longest_axis](
                                        const std::unique_ptr<Geometry>& a,
                                        const std::unique_ptr<Geometry>& b) {
            const float a_center = (a->bounding_box.min_corner(longest_axis) +
                                    a->bounding_box.max_corner(longest_axis)) /
                                   2;
            const float b_center = (b->bounding_box.min_corner(longest_axis) +
                                    b->bounding_box.max_corner(longest_axis)) /
                                   2;
            return a_center < b_center;
        };

        std::vector<std::unique_ptr<Geometry>> objs =
            objs_left.size() != 0 ? std::move(objs_left)
                                  : std::move(objs_right);

        const auto mid_iter =
            objs.begin() + static_cast<std::ptrdiff_t>(objs.size() / 2);
        std::nth_element(objs.begin(), mid_iter, objs.end(), compare_center);

        objs_left.clear();
        objs_left.insert(objs_left.end(), std::make_move_iterator(objs.begin()),
                         std::make_move_iterator(mid_iter));
        objs_right.clear();
        objs_right.insert(objs_right.end(), std::make_move_iterator(mid_iter),
                          std::make_move_iterator(objs.end()));
    }
    assert(objs_left.size() > 0 && objs_right.size() > 0);

    left = (objs_left.size() == 1)
               ? std::move(objs_left[0])
               : std::make_unique<AABBTree>(std::move(objs_left));
    right = (objs_right.size() == 1)
                ? std::move(objs_right[0])
                : std::make_unique<AABBTree>(std::move(objs_right));
}

Intersection AABBTree::intersect(const Ray& ray) const {
    if (!bounding_box.intersect(ray)) return Intersection::NoIntersection();

    const Intersection left_hit = left->intersect(ray);
    const Intersection right_hit = right->intersect(ray);

    return left_hit.earlier(right_hit);
}
