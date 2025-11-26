#include <omp.h>

#include <Eigen/Core>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

#include "ray_color.h"
#include "read_json.h"
#include "util/ProgressBar.h"
#include "util/Timer.h"
#include "viewing_ray.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " scene.json" << '\n';
        exit(EXIT_FAILURE);
    }

    const Scene scene = read_json(argv[1]);

    const int width = static_cast<int>(scene.camera.resolution_x);
    const int height = static_cast<int>(scene.camera.resolution_y);
    std::vector<unsigned char> image(static_cast<size_t>(3 * width * height));

    std::cout << "Rendering in threads: " << omp_get_max_threads() << '\n';
    {
        Timer timer("Render");
        ProgressBar progress_bar("Rendering", width * height);
#pragma omp parallel for collapse(2) schedule(dynamic)
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                // Compute viewing ray
                const Ray ray = viewing_ray(scene.camera, i, j, width, height);

                // Shoot ray and collect color
                Eigen::Vector3f rgb = ray_color(ray, scene);
                rgb *= scene.camera.exposure;
                rgb = rgb.cwiseMax(Eigen::Vector3f::Zero())
                          .cwiseMin(Eigen::Vector3f::Ones());

                Eigen::Vector3<unsigned char> pixels =
                    (std::numeric_limits<unsigned char>::max() * rgb)
                        .cast<unsigned char>();
                image[(3 * (j + width * i)) + 0] = pixels(0);
                image[(3 * (j + width * i)) + 1] = pixels(1);
                image[(3 * (j + width * i)) + 2] = pixels(2);

                progress_bar.update();
            }
        }
    }

    stbi_write_png("output.png", width, height, 3, image.data(), 3 * width);
}
