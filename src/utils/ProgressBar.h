#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <string>

class ProgressBar {
   private:
    static const int BAR_LENGTH = 30;

    const std::string name;
    const int total;
    int completed;
    int last_completed_length;

   public:
    ProgressBar(const std::string& name, const int total_count);
    void update();

   private:
    void display();
};

#endif
