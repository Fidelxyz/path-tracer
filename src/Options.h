#ifndef OPTIONS_H
#define OPTIONS_H

struct Options {
    // Number of samples per pixel.
    unsigned int samples;
    // Clamping value for each ray.
    float ray_clamp;
};

#endif
