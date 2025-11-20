#ifndef BLINN_PHONG_SHADING_H
#define BLINN_PHONG_SHADING_H

#include <Eigen/Core>

#include "Scene.h"

/**
 * Given a ray and its hit in the scene, return the Blinn-Phong shading
 * contribution over all _visible_ light sources (e.g., take into account
 * shadows). Use a hard-coded value of ia=0.1 for ambient light.
 *
 * @param ray incoming ray
 * @param intersection intersection information of the ray with the scene
 * @param normal unit surface normal at hit
 * @param scene the scene containing objects and lights
 * @return shaded color collected by this ray as rgb 3-vector
 */
Eigen::Vector3f blinn_phong_shading(const Ray& ray,
                                    const Intersection& intersection,
                                    const Eigen::Vector3f& normal,
                                    const Scene& scene);

#endif
