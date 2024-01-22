#include "pch.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <mutex>
#include <optional>

#include "QuietWindow.h"
#include "QuietWindow.g.cpp"

HRESULT SendStartSignal()
{
    return S_OK;
}

HRESULT SendStopSignal()
{
    return S_OK;
}


class TimeWindow
{
public:
    TimeWindow(std::chrono::seconds seconds)
    {
        m_startTime = std::chrono::steady_clock::now();
        m_duration = seconds;
        m_futureWindowCloser = std::async(std::launch::async, [this]() {
            while (!this->m_cancelled)
            {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - this->m_startTime);

                if (elapsed >= m_duration)
                {
                    // callback();
                    break;
                }

                // Sleep for a short duration to avoid busy waiting
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
    }

    void Cancel()
    {
        m_cancelled = true;
    }

    int64_t TimeLeftInSeconds()
    {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now - m_startTime).count();
    }

private:
    std::chrono::steady_clock::time_point m_startTime{};
    std::chrono::seconds m_duration{};
    std::future<void> m_futureWindowCloser;
    bool m_cancelled{};
};

std::mutex g_mutex;
std::optional<TimeWindow> g_activeTimeWindow;

void CloseQuietWindow()
{
    // g_isActive = false;
}

namespace winrt::QuietBackgroundProcesses_ElevatedServer::implementation
{
    int64_t QuietWindow::StartQuietWindow()
    {
        auto lock = std::scoped_lock(g_mutex);

        if (g_activeTimeWindow)
        {
            return g_activeTimeWindow.value().TimeLeftInSeconds();
        }

        // Start
        auto timeWindow = TimeWindow(std::chrono::seconds(6));
        g_activeTimeWindow = std::move(timeWindow);
        return g_activeTimeWindow.value().TimeLeftInSeconds();
    }

    void QuietWindow::StopQuietWindow()
    {
        auto lock = std::scoped_lock(g_mutex);
        if (g_activeTimeWindow)
        {
            SendStopSignal();
            
            // Detach and destruct the current time window
            g_activeTimeWindow.value().Cancel();
            g_activeTimeWindow = std::nullopt;
        }
    }

    bool QuietWindow::IsActive()
    {
        auto lock = std::scoped_lock(g_mutex);
        return g_activeTimeWindow.has_value();
    }

    int64_t QuietWindow::TimeLeftInSeconds()
    {
        auto lock = std::scoped_lock(g_mutex);
        if (!g_activeTimeWindow)
        {
            return 0;
        }
        return g_activeTimeWindow.value().TimeLeftInSeconds();
    }
}
