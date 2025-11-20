#ifndef VIEWING_RAY_H
#define VIEWING_RAY_H

#include "Camera.h"
#include "Ray.h"

/**
 * Construct a viewing ray given a camera and subscripts to a pixel.
 *
 * @param camera Perspective camera object.
 * @param i Pixel row index.
 * @param j Pixel column index.
 * @param width Number of pixels width of image.
 * @param height Number of pixels height of image.
 * @return Viewing ray starting at camera shooting through pixel. When t=1, then
 *         `ray.origin + t * ray.direction` should land exactly on the center of
 *         the pixel (`i`, `j`).
 */
Ray viewing_ray(const Camera& camera, const int i, const int j, const int width,
                const int height);

#endif
