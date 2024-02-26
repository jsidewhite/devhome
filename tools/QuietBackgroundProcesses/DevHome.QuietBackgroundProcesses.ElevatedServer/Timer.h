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

class Timer
{
public:
    Timer(std::chrono::seconds seconds, std::function<void()> callback) :
        m_callback(std::forward<std::function<void()>>(callback))
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

        //{
            //auto lock = std::unique_lock<std::mutex>(mutex);
            //comFinished = true;
        //}
        m_cancelCondition.notify_one();
    }

    int64_t TimeLeftInSeconds()
    {
        auto lock = std::scoped_lock(m_mutex);
        if (m_cancelled)
        {
            return 0;
        }

        auto secondsLeft = CalculateSecondsLeft();
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
        auto lock = std::unique_lock<std::mutex>(m_mutex);

        m_cancelCondition.wait_for(lock, m_duration, [this] {
            return this->m_cancelled;
        });

        // Do the callback
        this->m_callback();
    }

    std::chrono::steady_clock::time_point m_startTime{};
    std::chrono::seconds m_duration{};
    std::future<void> m_timerThreadFuture;
    std::mutex m_mutex;
    bool m_cancelled{};
    std::condition_variable m_cancelCondition;
    std::function<void()> m_callback;

#ifdef TRACK_SECONDS_LEFT
    int64_t m_secondsLeft = -1;
#endif
};
