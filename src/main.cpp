#include <Eigen/Core>
#include <memory>
#include <vector>

#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "raycolor.h"
#include "read_json.h"
#include "utils/ProgressBar.h"
#include "utils/Timer.h"
#include "viewing_ray.h"
#include "write_ppm.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <scene.json>\n", argv[0]);
    }

    Camera camera;
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<std::shared_ptr<Light>> lights;
    read_json(argv[1], camera, objects, lights);

    const unsigned int width = 1280;
    const unsigned int height = 720;
    std::vector<unsigned char> rgb_image(3 * width * height);

    {
        Timer timer("Render");
        ProgressBar progress_bar("Rendering", width * height);
#pragma omp parallel for collapse(2)
        for (unsigned int i = 0; i < height; i++) {
            for (unsigned int j = 0; j < width; j++) {
                // Set background color
                Eigen::Vector3f rgb(0, 0, 0);

                // Compute viewing ray
                Ray ray;
                viewing_ray(camera, i, j, width, height, ray);

                // Shoot ray and collect color
                raycolor(ray, 1.0, objects, lights, 0, rgb);

                // Write double precision color into image
                auto clamp = [](double s) {
                    return std::max(std::min(s, 1.), 0.);
                };
                rgb_image[0 + 3 * (j + width * i)] = 255. * clamp(rgb(0));
                rgb_image[1 + 3 * (j + width * i)] = 255. * clamp(rgb(1));
                rgb_image[2 + 3 * (j + width * i)] = 255. * clamp(rgb(2));

#pragma omp critical
                progress_bar.update();
            }
        }
    }

    write_ppm("rgb.ppm", rgb_image, width, height, 3);
}
