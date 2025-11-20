#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <string>

class Timer {
   public:
    explicit Timer(std::string name);
    ~Timer();
    void stop();

   private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start_time;
    bool is_stopped = false;
};

#endif
