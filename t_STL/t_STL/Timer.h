#pragma once

#include <chrono>

template< typename Resolution, typename Clock = std::chrono::high_resolution_clock >
class Timer
{
public:
    constexpr Timer() = default;

    void start()
    {
        startTime = Clock::now();
    }

    auto stop() const
    {
        return std::chrono::duration_cast< Resolution >( Clock::now() - startTime ).count();
    }
private:
    std::chrono::time_point< Clock > startTime;
};