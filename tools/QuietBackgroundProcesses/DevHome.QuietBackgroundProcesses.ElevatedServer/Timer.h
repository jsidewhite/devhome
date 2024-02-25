// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "pch.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <mutex>
#include <optional>
#include <thread>

#include "Utility.h"

#if _DEBUG || NDEBUG
#define TRACK_SECONDS_LEFT
#endif

template <typename CallbackT>
class Timer
{
public:
    Timer(std::chrono::seconds seconds, CallbackT&& callback) :
        m_callback(std::forward<CallbackT>(callback))
    {
        m_startTime = std::chrono::steady_clock::now();
        m_duration = seconds;
        m_timerThreadFuture = std::async(std::launch::async, &Timer::TimerThread, this);
    }

    Timer(Timer&& other) noexcept = default;
    Timer& operator=(Timer&& other) noexcept = default;

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    void Cancel()
    {
        auto lock = std::scoped_lock(m_mutex);
        m_cancelled = true;

        OutputDebugStringW(L"Timer: Cancelled\n");
    }

    int64_t TimeLeftInSeconds()
    {
        auto lock = std::scoped_lock(m_mutex);
        if (m_cancelled)
        {
            return 0;
        }

        auto secondsLeft = CalculateSecondsLeft();
#ifdef TRACK_SECONDS_LEFT
        m_secondsLeft = secondsLeft;
#endif
        return std::max(secondsLeft, 0ll);
    }

private:
    int64_t CalculateSecondsLeft()
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime);
        auto secondsLeft = m_duration.count() - elapsed.count();
#ifdef TRACK_SECONDS_LEFT
        m_secondsLeft = secondsLeft;
#endif
        return secondsLeft;
    }

    void TimerThread()
    {
        // Pause until timer expired or cancelled
        while (true)
        {
            auto secondsLeft = CalculateSecondsLeft();
            if (secondsLeft <= 0 || this->m_cancelled)
            {
                break;
            }

            // Sleep for a short duration to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }

        // Do the callback
        auto lock = std::scoped_lock(m_mutex);
        if (!this->m_cancelled)
        {
            this->m_callback();
        }
    }

    std::chrono::steady_clock::time_point m_startTime{};
    std::chrono::seconds m_duration{};
    std::future<void> m_timerThreadFuture;
    std::mutex m_mutex;
    std::atomic<bool> m_cancelled{};
    CallbackT m_callback;

#ifdef TRACK_SECONDS_LEFT
    int64_t m_secondsLeft = -1;
#endif
};
