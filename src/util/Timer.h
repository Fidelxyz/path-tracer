#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <string>

class Timer {
   public:
    Timer(const Timer&) = default;
    Timer(Timer&&) = delete;
    Timer& operator=(const Timer&) = default;
    Timer& operator=(Timer&&) = delete;
    explicit Timer(std::string name);
    ~Timer();
    void stop();

   private:
    std::string name;
    std::chrono::high_resolution_clock::time_point start_time;
    bool is_stopped = false;
};

#endif
