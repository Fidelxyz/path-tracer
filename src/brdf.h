#ifndef BRDF_H
#define BRDF_H

#include <Eigen/Core>

#include "Intersection.h"
#include "Ray.h"

/**
 * Given a ray and its hit in the scene, return the Disney Principled BRDF
 * contribution over all _visible_ light sources (e.g., take into account
 * shadows). Use a hard-coded value of ia=0.1 for ambient light.
 *
 * @param view_to_surface ray from the camera/viewpoint to the surface hit.
 * @param surface_to_light ray from the surface hit to the light source.
 * @param view_intersection intersection information of the view ray.
 * @param normal Unit normal vector at the surface hit point.
 * @return The BRDF value.
 */
Eigen::Vector3f brdf(const Ray& view_to_surface, const Ray& surface_to_light,
                     const Intersection& view_intersection,
                     const Eigen::Vector3f& normal);

#endif
