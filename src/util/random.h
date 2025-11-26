#ifndef RANDOM_H
#define RANDOM_H

#include <random>

extern std::random_device rd;
extern std::minstd_rand rng;
extern std::uniform_real_distribution<float> uniform;

#endif
