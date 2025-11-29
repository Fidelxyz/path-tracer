#include "render.h"

#include <omp.h>

#include <iostream>

#include "path_tracing.h"
#include "util/ProgressBar.h"
#include "util/Timer.h"
#include "viewing_ray.h"

std::vector<Eigen::Vector3f> render(const Scene& scene) {
    const int width = static_cast<int>(scene.camera.resolution_x);
    const int height = static_cast<int>(scene.camera.resolution_y);
    std::vector<Eigen::Vector3f> image(static_cast<size_t>(width * height),
                                       Eigen::Vector3f::Zero());

    std::cout << "Rendering in threads: " << omp_get_max_threads() << '\n';
    Timer timer("Render");
    ProgressBar progress_bar("Rendering", width * height);

#pragma omp parallel for collapse(2) schedule(dynamic)
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            for (unsigned int s = 0; s < scene.camera.samples; s++) {
                Ray ray = viewing_ray(scene.camera, i, j, width, height);
                Eigen::Vector3f rgb = sample(ray, scene);
                image[i * width + j] += rgb;
            }
            image[i * width + j] /= static_cast<float>(scene.camera.samples);
            image[i * width + j] *= scene.camera.exposure;

            progress_bar.update();
        }
    }

    return image;
}
