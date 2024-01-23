#include "pch.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <mutex>
#include <optional>

using CallbackFunction = void (*)();

class Timer
{
public:
    Timer(std::chrono::seconds seconds, CallbackFunction callback)
    {
        m_startTime = std::chrono::steady_clock::now();
        m_duration = seconds;
        m_callback = callback;
        m_timerThreadFuture = std::async(std::launch::async, &Timer::TimerThread, this);
    }

    Timer(Timer&& other)
    {
        m_startTime = std::move(other.m_startTime);
        m_duration = std::move(other.m_duration);
        m_callback = std::move(other.m_callback);
        m_timerThreadFuture = std::move(other.m_timerThreadFuture);
    }

    Timer& operator=(Timer&& other)
    {
        m_startTime = std::move(other.m_startTime);
        m_duration = std::move(other.m_duration);
        m_callback = std::move(other.m_callback);
        m_timerThreadFuture = std::move(other.m_timerThreadFuture);
        return *this;
    }

    static void Destroy(Timer&& timeWindow)
    {
        // Destruct time window on sepearate thread because its destructor may take time to end (the std::future member is blocking)
        auto th = std::thread([timeWindow = std::move(timeWindow)]() {});
        th.detach();
    }


    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    void Cancel()
    {
        auto lock = std::scoped_lock(m_mutex);
        m_cancelled = true;
    }

    int64_t TimeLeftInSeconds()
    {
        auto lock = std::scoped_lock(m_mutex);
        if (m_cancelled)
        {
            return 0;
        }
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime).count();
    }

private:
    void TimerThread()
    {
        while (!this->m_cancelled)
        {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - this->m_startTime);

            if (elapsed >= m_duration)
            {
                auto lock = std::scoped_lock(m_mutex);
                if (!this->m_cancelled)
                {
                    this->m_callback();
                }
                break;
            }

            // Sleep for a short duration to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    bool m_callbackCalled{};
    std::chrono::steady_clock::time_point m_startTime{};
    std::chrono::seconds m_duration{};
    std::future<void> m_timerThreadFuture;
    std::mutex m_mutex;
    std::atomic<bool> m_cancelled{};
    CallbackFunction m_callback;
};
