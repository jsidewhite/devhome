#include "pch.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <mutex>
#include <optional>

#include "TimeWindow.h"
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
        auto timeWindow = TimeWindow(std::chrono::seconds(6), []()
        {
            SendStopSignal();
        });
        *g_activeTimeWindow = std::move(timeWindow);
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
