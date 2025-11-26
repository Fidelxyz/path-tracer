#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <string>

class ProgressBar {
   public:
    ProgressBar(std::string name, int total_count);
    void update();

   private:
    void display();

    static const int BAR_LENGTH = 30;

    std::string name;
    int total;
    int completed;
    int last_completed_length;
};

#endif
