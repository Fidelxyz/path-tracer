#include <unistd.h>

#include <Eigen/Core>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "reader/read_json.h"
#include "render.h"

static const float GAMMA = 2.2F;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0]
                  << " <scene.json> [-o <output.png>]\n";
        exit(EXIT_FAILURE);
    }

    std::string output_filename = "output.png";

    int c = 0;
    while ((c = getopt(argc, argv, "o:")) != -1) {
        switch (c) {
            case 'o':
                output_filename = std::string(optarg);
                break;
            case '?':
                exit(EXIT_FAILURE);
            default:
                abort();
        }
    }
    if (optind >= argc) {
        std::cerr << "No input JSON scene file provided.\n";
        exit(EXIT_FAILURE);
    }

    const Scene scene = read_json(argv[optind]);

    const int width = static_cast<int>(scene.camera.resolution_x);
    const int height = static_cast<int>(scene.camera.resolution_y);

    std::vector<Eigen::Vector3f> image = render(scene);
    std::vector<unsigned char> output(image.size() * 3);

    for (size_t i = 0; i < image.size(); i++) {
        auto color = image[i]
                         .cwisePow(1.F / GAMMA)
                         .cwiseMax(Eigen::Vector3f::Zero())
                         .cwiseMin(Eigen::Vector3f::Ones());

        const Eigen::Vector3<unsigned char> pixel =
            (std::numeric_limits<unsigned char>::max() * color)
                .cast<unsigned char>();

        output[3 * i + 0] = pixel(0);
        output[3 * i + 1] = pixel(1);
        output[3 * i + 2] = pixel(2);
    }

    stbi_write_png(output_filename.c_str(), width, height, 3, output.data(),
                   3 * width);
}
