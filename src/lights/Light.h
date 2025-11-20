#ifndef LIGHT_H
#define LIGHT_H

#include <Eigen/Core>

#include "../Ray.h"

class Light {
   public:
    Light(Eigen::Vector3f intensity);
    // https://stackoverflow.com/questions/461203/when-to-use-virtual-destructors
    virtual ~Light() {};

    /**
     * Given a query point return a ray _toward_ the Light.
     *
     * @param [in] point 3D query point in space
     * @return Ray from `point` toward the Light
     */
    virtual Ray ray_from(Eigen::Vector3f point) const = 0;

    const Eigen::Vector3f intensity;
};
#endif
