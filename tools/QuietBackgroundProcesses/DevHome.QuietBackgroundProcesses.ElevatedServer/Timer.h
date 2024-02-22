// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

#include "pch.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <mutex>
#include <optional>

#include <wrl/module.h>
#include <wil/resource.h>

using CallbackFunction = void (*)();

using unique_com_server_process_ref = wil::unique_call<decltype(&::CoReleaseServerProcess), ::CoReleaseServerProcess>;

class Timer
{
public:
    // Cleanup functions
    static void Discard(std::unique_ptr<Timer> timer);
    static void WaitForAllDiscardedTimersToDestruct();

    Timer(std::chrono::seconds seconds, CallbackFunction callback)
    {
        // CoAddRefServerProcess();
        auto& module = Microsoft::WRL::Module<Microsoft::WRL::OutOfProc>::GetModule();
        auto count = module.IncrementObjectCount();

        auto msg = std::wstring(L"Timer: Timer::Timer CoAddRefServerProcess = ") + std::to_wstring(count) + std::wstring(L"\n");
        OutputDebugStringW(msg.c_str());


        m_startTime = std::chrono::steady_clock::now();
        m_duration = seconds;
        m_callback = std::move(callback);
        m_timerThreadFuture = std::async(std::launch::async, &Timer::TimerThread, this);
    }

    ~Timer()
    {
        auto msg = std::wstring(L"Timer: Timer::~Timer\n");
        OutputDebugStringW(msg.c_str());
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
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime);

        auto left = m_duration.count() - elapsed.count();
#if _DEBUG
        secondsLeft = left;
#endif
        return std::max(left, 0ll);
    }

private:
    void TimerThread()
    {
        // Pause until timer expired or cancelled
        while (true)
        {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - this->m_startTime);

            if (this->m_cancelled || elapsed >= m_duration)
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

        auto& module = Microsoft::WRL::Module<Microsoft::WRL::OutOfProc>::GetModule();
        auto count = module.DecrementObjectCount();
        auto msg = std::wstring(L"Timer: CoReleaseServerProcess = ") + std::to_wstring(count) + std::wstring(L"\n");
        OutputDebugStringW(msg.c_str());
        //m_serverReference.reset();
    }

    std::chrono::steady_clock::time_point m_startTime{};
    std::chrono::seconds m_duration{};
    std::future<void> m_timerThreadFuture;
    std::mutex m_mutex;
    std::atomic<bool> m_cancelled{};
    CallbackFunction m_callback;
    unique_com_server_process_ref m_serverReference;

#if _DEBUG
    int64_t m_secondsLeft = -1;
#endif
};
