#pragma once

#include <chrono> // for std::chrono functions
#include <iomanip> // for std::setprecision
#include <iostream> // for std::cout

class Timer
{
private:
    // Type aliases to make accessing nested type easier
    using Clock = std::chrono::steady_clock;
    using Milliseconds = std::chrono::duration<double, std::milli>;

    std::chrono::time_point<Clock> m_beg { Clock::now() };

public:
    void reset()
    {
        m_beg = Clock::now();
    }

    // Return elapsed time in milliseconds with high precision
    double elapsed() const
    {
        return std::chrono::duration_cast<Milliseconds>(Clock::now() - m_beg).count();
    }

    int roundedElapsed() const
    {
        return std::chrono::duration_cast<Milliseconds>(Clock::now() - m_beg).count();
    }
};