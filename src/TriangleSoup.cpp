#include "TriangleSoup.h"

#include "Ray.h"
#include "first_hit.h"

bool TriangleSoup::intersect(const Ray& ray, const double min_t, double& t,
                             Eigen::Vector3f& n) const {
    int hit_id;
    return first_hit(ray, min_t, this->triangles, hit_id, t, n);
}
