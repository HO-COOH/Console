#include <chrono>

class ScopedTimer
{
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    const std::chrono::steady_clock::duration duration{};
public:
    ScopedTimer(unsigned int microseconds) :duration{ std::chrono::microseconds{microseconds} }
    {
    }
    void wait()
    {
        while (std::chrono::steady_clock::now() <= start + duration)
            ;
        //std::this_thread::sleep_until(start + duration);
        start = std::chrono::steady_clock::now();
    }
};
