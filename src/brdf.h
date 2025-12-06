#ifndef BRDF_H
#define BRDF_H

#include <Eigen/Core>

#include "Intersection.h"
#include "Ray.h"

/**
 * Compute the Cook-Torrance BRDF value.
 *
 * @param view_to_surface ray from the source to the surface hit.
 * @param surface_to_light ray from the surface hit to the light source.
 * @param view_intersection intersection information of the view ray.
 * @param normal Unit normal vector at the surface hit point.
 * @param texcoords Texture coordinates at the surface hit point.
 * @return The BRDF value.
 */
Eigen::Vector3f brdf(const Ray& view_to_surface, const Ray& surface_to_light,
                     const Intersection& view_intersection,
                     const Eigen::Vector3f& normal,
                     const Eigen::Vector2f& texcoords);

/**
 * Sample a direction according to the importance distribution defined by the
 * Cook-Torrance BRDF.
 *
 * @param view_to_surface ray from the source to the surface hit.
 * @param view_intersection intersection information of the view ray.
 * @param surface_point Position of the surface hit point.
 * @param normal Unit normal vector at the surface hit point.
 * @param texcoords Texture coordinates at the surface hit point.
 * @return A ray starting from the surface point in the sampled direction.
 */
Ray brdf_sample(const Ray& view_to_surface,
                const Intersection& view_intersection,
                const Eigen::Vector3f& surface_point,
                const Eigen::Vector3f& normal,
                const Eigen::Vector2f& texcoords);

/**
 * Compute the PDF of sampling a given direction according to the importance
 * distribution defined by the Cook-Torrance BRDF.
 *
 * @param view_to_surface ray from the source to the surface hit.
 * @param surface_to_light ray from the surface hit to the light source.
 * @param view_intersection intersection information of the view ray.
 * @param normal Unit normal vector at the surface hit point.
 * @param texcoords Texture coordinates at the surface hit point.
 * @return The PDF value.
 */
float brdf_pdf(const Ray& view_to_surface, const Ray& surface_to_light,
               const Intersection& view_intersection,
               const Eigen::Vector3f& normal, const Eigen::Vector2f& texcoords);

#endif
