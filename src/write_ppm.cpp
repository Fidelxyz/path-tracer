#include "write_ppm.h"

#include <cassert>
#include <fstream>
#include <iostream>

bool write_ppm(const std::string& filename,
               const std::vector<unsigned char>& data, const int width,
               const int height, const int num_channels) {
    assert((num_channels == 3 || num_channels == 1) &&
           ".ppm only supports RGB or grayscale images");

    std::ofstream ofs(filename);

    if (num_channels == 1) {
        ofs << "P2" << "\n";
        ofs << width << " " << height << "\n";
        ofs << 255 << "\n";

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                ofs << std::to_string(data[i * width + j]);
                ofs << (j == width - 1 ? "\n" : " ");
            }
        }

    } else if (num_channels == 3) {
        ofs << "P3" << "\n";
        ofs << width << " " << height << "\n";
        ofs << 255 << "\n";

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                ofs << std::to_string(data[(i * width + j) * 3 + 0]) << " ";
                ofs << std::to_string(data[(i * width + j) * 3 + 1]) << " ";
                ofs << std::to_string(data[(i * width + j) * 3 + 2])
                    << (j == width - 1 ? "\n" : " ");
            }
        }

    } else {
        std::cerr << "Error: num_channels must be 1 or 3." << std::endl;
        return false;
    }

    return true;
}
