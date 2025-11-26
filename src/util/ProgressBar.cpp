#include "ProgressBar.h"

#include <iostream>

ProgressBar::ProgressBar(std::string name, const int total_count)
    : name(std::move(name)),
      total(total_count),
      completed(0),
      last_completed_length(-1) {
    display();
}

void ProgressBar::update() {
    if (completed >= total) {
        return;
    }
#pragma omp atomic
    completed++;
    display();
}

void ProgressBar::display() {
    const int completed_length = static_cast<int>(
        static_cast<float>(BAR_LENGTH) *
        (static_cast<float>(completed) / static_cast<float>(total)));
    if (completed_length == last_completed_length) {
        return;
    }

#pragma omp critical
    {
        last_completed_length = completed_length;

        std::cout << "\r" << name << " [";
        for (int i = 0; i < completed_length; i++) {
            std::cout << '=';
        }
        for (int i = BAR_LENGTH - completed_length; i > 0; i--) {
            std::cout << ' ';
        }
        std::cout << "] " << completed << " / " << total;
        if (completed == total) {
            std::cout << "\n";
        }
        std::cout << std::flush;
    }
}
