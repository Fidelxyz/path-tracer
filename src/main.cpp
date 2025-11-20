#include <Eigen/Core>
#include <cstdlib>
#include <vector>

#include "ray_color.h"
#include "read_json.h"
#include "utils/ProgressBar.h"
#include "utils/Timer.h"
#include "utils/write_ppm.h"
#include "viewing_ray.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <scene.json>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const Scene scene = read_json(argv[1]);

    const unsigned int width = 1280;
    const unsigned int height = 720;
    std::vector<unsigned char> image(3 * width * height);

    {
        Timer timer("Render");
        ProgressBar progress_bar("Rendering", width * height);
#pragma omp parallel for collapse(2) schedule(dynamic)
        for (unsigned int i = 0; i < height; i++) {
            for (unsigned int j = 0; j < width; j++) {
                // Compute viewing ray
                const Ray ray = viewing_ray(scene.camera, i, j, width, height);

                // Shoot ray and collect color
                const Eigen::Vector3f rgb = ray_color(ray, scene);

                // Write color into image
                const auto clamp = [](const float s) {
                    return std::max(std::min(s, 1.f), 0.f);
                };
                image[3 * (j + width * i) + 0] = 255.f * clamp(rgb(0));
                image[3 * (j + width * i) + 1] = 255.f * clamp(rgb(1));
                image[3 * (j + width * i) + 2] = 255.f * clamp(rgb(2));

                progress_bar.update();
            }
        }
    }

    write_ppm("rgb.ppm", image, width, height, RGB);
}
