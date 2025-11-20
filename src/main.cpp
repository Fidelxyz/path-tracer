#include <Eigen/Core>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

#include "ray_color.h"
#include "read_json.h"
#include "utils/ProgressBar.h"
#include "utils/Timer.h"
#include "utils/write_ppm.h"
#include "viewing_ray.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " scene.json" << '\n';
        exit(EXIT_FAILURE);
    }

    const Scene scene = read_json(argv[1]);

    const int width = 1280;
    const int height = 720;
    std::vector<unsigned char> image(static_cast<size_t>(3 * width * height));

    {
        Timer timer("Render");
        ProgressBar progress_bar("Rendering", width * height);
#pragma omp parallel for collapse(2) schedule(dynamic)
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                // Compute viewing ray
                const Ray ray = viewing_ray(scene.camera, i, j, width, height);

                // Shoot ray and collect color
                const Eigen::Vector3f rgb = ray_color(ray, scene);

                // Write color into image
                const auto image_value =
                    [](const float value) -> unsigned char {
                    const float clamped = std::max(std::min(value, 1.F), 0.F);
                    return static_cast<unsigned char>(
                        std::numeric_limits<unsigned char>::max() * clamped);
                };
                image[(3 * (j + width * i)) + 0] = image_value(rgb(0));
                image[(3 * (j + width * i)) + 1] = image_value(rgb(1));
                image[(3 * (j + width * i)) + 2] = image_value(rgb(2));

                progress_bar.update();
            }
        }
    }

    write_ppm("rgb.ppm", image, width, height, Channels::RGB);
}
