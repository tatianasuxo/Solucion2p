#pragma once

#include <chrono>

template<typename F, typename... Args>
double measureExecution(F func, Args&&... args)
{
    std::chrono::high_resolution_clock::time_point timePnt = std::chrono::high_resolution_clock::now();
    func(std::forward<Args>(args)...);
    return static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - timePnt).count());
}

class Timer
{
private:
    std::chrono::high_resolution_clock::time_point timePnt;

public:
    inline void start()
    {
        timePnt = std::chrono::high_resolution_clock::now();
    }

    inline double stopNs()
    {
        return static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - timePnt).count());
    }
};