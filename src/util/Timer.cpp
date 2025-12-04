#include "Timer.h"

#include <iostream>

Timer::Timer(std::string name)
    : name(std::move(name)),
      start_time(std::chrono::high_resolution_clock::now()) {}

Timer::~Timer() { stop(); }

void Timer::stop() {
    if (is_stopped) {
        return;
    }
    is_stopped = true;
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
        end_time - start_time);
    std::cout << "[Timer] " << name << ": "
              << static_cast<float>(duration.count()) * (1.F / 1e6F) << " ms\n";
}
