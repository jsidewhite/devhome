#include "pch.h"

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>

#include <wil/resource.h>
#include <wil/win32_helpers.h>
#include <wil/result_macros.h>

#include "Timer.h"
#include "QuietState.h"
#include "QuietWindow.h"
#include "QuietWindow.g.cpp"

std::mutex g_mutex;
std::unique_ptr<Timer> g_activeTimer;

//std::unique_ptr<QuietWindowState::QuietWindowStateEnable> g_quietWindowState;
QuietState::unique_quietwindowclose_call g_quietState;

namespace winrt::QuietBackgroundProcesses_ElevatedServer::implementation
{
    int64_t QuietWindow::StartQuietWindow()
    {
        auto lock = std::scoped_lock(g_mutex);

        // Clean up old timer windows
        if (g_activeTimer && g_activeTimer->IsFinished())
        {
            g_activeTimer.reset();
        }

        if (g_activeTimer)
        {
            return g_activeTimer->TimeLeftInSeconds();
        }

        // Start
        g_activeTimer.reset(new Timer(std::chrono::seconds(6), []()
        {
            auto lock = std::scoped_lock(g_mutex);
            g_quietState.reset();
        }));

        //*g_quietWindowState = QuietWindowState::QuietWindowStateEnable::Enable();
        g_quietState = QuietState::turnOn();
        return g_activeTimer->TimeLeftInSeconds();
    }

    void QuietWindow::StopQuietWindow()
    {
        auto lock = std::scoped_lock(g_mutex);
        if (!g_activeTimer || g_activeTimer->IsFinished())
        {
            return;
        }

        // Turn off the quiet window no matter what
        auto signalStop = wil::scope_exit([]() {
            g_quietState.reset();
        });

        // Detach and destruct the current time window
        g_activeTimer->Cancel();
        Timer::Destroy(std::move(*g_activeTimer));

        g_activeTimer = nullptr;
    }

    bool QuietWindow::IsActive()
    {
        auto lock = std::scoped_lock(g_mutex);
        return g_activeTimer && !g_activeTimer->IsFinished();
    }

    int64_t QuietWindow::TimeLeftInSeconds()
    {
        auto lock = std::scoped_lock(g_mutex);
        if (!g_activeTimer || g_activeTimer->IsFinished())
        {
            return 0;
        }
        return g_activeTimer->TimeLeftInSeconds();
    }
}
