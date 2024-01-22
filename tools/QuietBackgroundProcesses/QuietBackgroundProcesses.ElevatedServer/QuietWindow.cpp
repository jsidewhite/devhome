#include "pch.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <mutex>
#include <optional>

#include <wil/resource.h>
#include <wil/win32_helpers.h>
#include <wil/result_macros.h>

#include "TimeWindow.h"
#include "QuietWindowState.h"
#include "QuietWindow.h"
#include "QuietWindow.g.cpp"

std::mutex g_mutex;
std::optional<TimeWindow> g_activeTimeWindow;

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
            QuietWindowState::TurnOff();
        });
        *g_activeTimeWindow = std::move(timeWindow);

        QuietWindowState::TurnOn();
        return g_activeTimeWindow.value().TimeLeftInSeconds();
    }

    void QuietWindow::StopQuietWindow()
    {
        auto lock = std::scoped_lock(g_mutex);
        if (!g_activeTimeWindow)
        {
            return;
        }

        // Turn off the quiet window no matter what
        auto signalStop = wil::scope_exit([]() {
            QuietWindowState::TurnOff();
        });

        // Detach and destruct the current time window
        TimeWindow oldWindow = std::move(g_activeTimeWindow.value());
        oldWindow.Cancel();

        // Destruct old window on sepearate thread because its destructor may take time to end (the std::future member is blocking)
        std::thread([oldWindow = std::move(oldWindow)]() {
        }).detach();

        g_activeTimeWindow = std::nullopt;
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
