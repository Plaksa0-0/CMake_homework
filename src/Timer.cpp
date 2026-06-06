#include "Timer.h"

#include <iostream>

namespace NBenchmark
{

CTimer::CTimer()
    : m_startTime(std::chrono::high_resolution_clock::now())
{
}

CTimer::CTimer(const char* label)
    : m_startTime(std::chrono::high_resolution_clock::now()),
      m_label(label)
{
}

CTimer::~CTimer()
{
    const auto duration = GetElapsedMicroseconds();
    if (m_label)
    {
        std::cout << m_label << ": " << duration << " microseconds" << std::endl;
    }
    else
    {
        std::cout << "Duration: " << duration << " microseconds" << std::endl;
    }
}

long long CTimer::GetElapsedMicroseconds() const
{
    const auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_startTime).count();
}

} // namespace NBenchmark
