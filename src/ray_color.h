#ifndef RAYCOLOR_H
#define RAYCOLOR_H

#include <Eigen/Core>

#include "Scene.h"

/**
 * Shoots a ray into a lit scene and collects color information.
 *
 * @param ray The ray along which to search.
 * @param scene The scene containing objects and lights.
 *
 * @return The collected color as an Eigen::Vector3f.
 */
Eigen::Vector3f ray_color(const Ray& ray, const Scene& scene);

#endif
