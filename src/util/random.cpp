#include "random.h"

#include <random>

std::random_device rd;
std::minstd_rand rng = std::minstd_rand(rd());
