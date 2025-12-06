#ifndef PATH_TRACING_H
#define PATH_TRACING_H

#include <Eigen/Core>

#include "Scene.h"

/**
 * Samples the color along a ray in the scene.
 *
 * @param ray The ray to be traced.
 * @param scene The scene.
 *
 * @return The sampled color.
 */
Eigen::Vector3f sample(const Ray& ray, const Scene& scene);

#endif
