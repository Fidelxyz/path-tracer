#ifndef RENDER_H
#define RENDER_H

#include <Eigen/Core>

#include "Scene.h"

std::vector<Eigen::Vector3f> render(const Scene& scene);

#endif
