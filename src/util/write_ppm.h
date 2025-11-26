#ifndef WRITE_PPM_H
#define WRITE_PPM_H

#include <string>
#include <vector>

enum Channels : std::uint8_t { GRAY, RGB };

/**
 * Write an rgb or grayscale image to a .ppm file.
 *
 * @param filename path to .ppm file as string
 * @param data `width * heigh * num_channels` array of image intensity data
 * @param width image width (i.e., number of columns)
 * @param height image height (i.e., number of rows)
 * @param channels channels type (RGB or GRAY)
 * @return true on success, false on failure
 */
bool write_ppm(const std::string& filename,
               const std::vector<unsigned char>& data, const int width,
               const int height, const Channels channels);

#endif
