#pragma once

#include <chrono>

namespace NBenchmark
{

class CTimer
{
public:
    CTimer();
    explicit CTimer(const char* label);

    ~CTimer();

    long long GetElapsedMicroseconds() const;

private:
    std::chrono::high_resolution_clock::time_point m_startTime;
    const char* m_label = nullptr;
};

} // namespace NBenchmark