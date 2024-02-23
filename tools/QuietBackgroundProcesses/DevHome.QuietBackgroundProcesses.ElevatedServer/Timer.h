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

#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager.h"

#if _DEBUG || NDEBUG
#define TRACK_SECONDS_LEFT
#endif

class Timer
{
public:
    // Cleanup functions
    static void Discard(std::unique_ptr<Timer> timer);
    static std::thread GetDiscardThread();

    wil::com_ptr<ABI::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionManagerStatics> m_factory;

    Timer(std::chrono::seconds seconds, std::function<void()> callback)
    {
        m_startTime = std::chrono::steady_clock::now();
        m_duration = seconds;
        m_callback = std::move(callback);
        m_timerThreadFuture = std::async(std::launch::async, &Timer::TimerThread, this);

        m_factory = wil::GetActivationFactory<ABI::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionManagerStatics>(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSessionManager);
    }

    Timer(Timer&& other) noexcept = default;
    Timer& operator=(Timer&& other) noexcept = default;

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    void Cancel()
    {
        auto lock = std::scoped_lock(m_mutex);
        m_cancelled = true;

        auto msg = std::wstring(L"Timer: Cancelled\n");
        OutputDebugStringW(msg.c_str());
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

        m_serverReference.reset();
        m_factory.reset();
    }

    std::chrono::steady_clock::time_point m_startTime{};
    std::chrono::seconds m_duration{};
    std::future<void> m_timerThreadFuture;
    std::mutex m_mutex;
    std::atomic<bool> m_cancelled{};
    std::function<void()> m_callback;
    wrl_server_process_ref m_serverReference;

#ifdef TRACK_SECONDS_LEFT
    int64_t m_secondsLeft = -1;
#endif
};
