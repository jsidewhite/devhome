#include "pch.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

#include <wil/resource.h>
#include <wil/win32_helpers.h>
#include <wil/result_macros.h>

#include "Timer.h"
#include "QuietState.h"
#include "QuietWindow.h"
#include "QuietWindow.g.cpp"

std::mutex g_mutex;
std::unique_ptr<Timer> g_activeTimer;
std::vector<Timer> g_discardedTimers;

//std::unique_ptr<QuietWindowState::QuietWindowStateEnable> g_quietWindowState;
QuietState::unique_quietwindowclose_call g_quietState;

static void CleanupDiscardedTimers()
{
    // g_discardedTimers
}

static void DiscardTimer(Timer* timer)
{
    if (!timer)
    {
        return;
    }
    timer->Cancel();
    g_discardedTimers.push_back(std::move(*timer));
}

namespace winrt::QuietBackgroundProcesses_ElevatedServer::implementation
{
    int64_t QuietWindow::StartQuietWindow()
    {
        auto lock = std::scoped_lock(g_mutex);

        // Clean up old timers
        DiscardTimer(g_activeTimer.release());
        CleanupDiscardedTimers();

        // Start timer
        g_activeTimer.reset(new Timer(std::chrono::seconds(6), []()
        {
            auto lock = std::scoped_lock(g_mutex);
            g_quietState.reset();
        }));

        // Turn on quiet mode
        g_quietState = QuietState::TurnOn();

        // Return duration for showing countdown
        return g_activeTimer->TimeLeftInSeconds();
    }

    void QuietWindow::StopQuietWindow()
    {
        auto lock = std::scoped_lock(g_mutex);

        // Turn off quiet mode
        g_quietState.reset();

        // Detach and destruct the current time window
        DiscardTimer(g_activeTimer.release());
    }

    bool QuietWindow::IsActive()
    {
        auto lock = std::scoped_lock(g_mutex);
        return !!g_quietState;
    }

    int64_t QuietWindow::TimeLeftInSeconds()
    {
        auto lock = std::scoped_lock(g_mutex);
        if (!g_quietState || !g_activeTimer)
        {
            return 0;
        }
        return g_activeTimer->TimeLeftInSeconds();
    }
}
