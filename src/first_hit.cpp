#include "first_hit.h"

bool first_hit(const Ray& ray, const double min_t,
               const std::vector<std::shared_ptr<Object> >& objects,
               int& out_hit_id, double& out_t, Eigen::Vector3f& out_n) {
    int hit_id = -1;
    double t = std::numeric_limits<decltype(t)>::max();
    Eigen::Vector3f n;

    for (size_t i = 0; i < objects.size(); i++) {
        double this_t;
        Eigen::Vector3f this_n;
        if (objects[i]->intersect(ray, min_t, this_t, this_n)) {
            if (this_t < t) {
                t = this_t;
                n = this_n;
                hit_id = i;
            }
        }
    }

    if (hit_id == -1) return false;

    out_hit_id = hit_id;
    out_t = t;
    out_n = std::move(n);
    return true;
}
