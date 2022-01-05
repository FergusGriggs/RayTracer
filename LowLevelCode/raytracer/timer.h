#pragma once

#include <chrono>

class Timer
{
    using timep_t = typename std::chrono::steady_clock::time_point;
    timep_t m_start = std::chrono::steady_clock::now(), m_end = {};

public:
    void tick()
    {
        m_end = timep_t{};
        m_start = std::chrono::steady_clock::now();
    }

    void tock()
    {
        m_end = std::chrono::steady_clock::now();
    }

    std::chrono::microseconds duration() const
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(m_end - m_start);
    }
};