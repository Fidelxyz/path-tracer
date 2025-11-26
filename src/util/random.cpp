#include "random.h"

#include <random>

std::random_device rd;
std::minstd_rand rng = std::minstd_rand(rd());
std::uniform_real_distribution<float> uniform =
    std::uniform_real_distribution<float>(0.0F, 1.0F);
