#include "ProgressBar.h"

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

        printf("\r%s [", name.c_str());
        for (int i = 0; i < completed_length; i++) {
            putchar('=');
        }
        for (int i = BAR_LENGTH - completed_length; i > 0; i--) {
            putchar('-');
        }
        printf("] %d / %d", completed, total);
        if (completed == total) {
            putchar('\n');
        }
        fflush(stdout);
    }
}
