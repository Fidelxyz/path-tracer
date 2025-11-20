#include "write_ppm.h"

#include <fstream>
#include <iostream>

bool write_ppm(const std::string& filename,
               const std::vector<unsigned char>& data, const int width,
               const int height, const Channels channels) {
    std::ofstream ofs(filename);
    if (!ofs) {
        std::cerr << "Error opening file for writing: " << filename
                  << std::endl;
        return false;
    }

    switch (channels) {
        case GRAY:
            ofs << "P2" << "\n";
            ofs << width << " " << height << "\n";
            ofs << 255 << "\n";

            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    ofs << std::to_string(data[i * width + j]);
                    ofs << (j == width - 1 ? "\n" : " ");
                }
            }
            break;

        case RGB:
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
            break;
    }

    return true;
}
