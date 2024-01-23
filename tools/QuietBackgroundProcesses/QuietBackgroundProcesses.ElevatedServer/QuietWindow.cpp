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
#include "QuietWindowState.h"
#include "QuietWindow.h"
#include "QuietWindow.g.cpp"

std::mutex g_mutex;
std::unique_ptr<Timer> g_activeTimer;
bool g_timerFinished{};

//std::unique_ptr<QuietWindowState::QuietWindowStateEnable> g_quietWindowState;
QuietWindowState::unique_quietwindowclose_call g_quietWindowState;

namespace winrt::QuietBackgroundProcesses_ElevatedServer::implementation
{
    int64_t QuietWindow::StartQuietWindow()
    {
        auto lock = std::scoped_lock(g_mutex);

        // Clean up old timer windows
        if (g_timerFinished)
        {
            Timer::Destroy(std::move(*g_activeTimer));
        }

        if (g_activeTimer)
        {
            return g_activeTimer->TimeLeftInSeconds();
        }

        // Start
        g_activeTimer.reset(new Timer(std::chrono::seconds(6), []()
        {
            auto lock = std::scoped_lock(g_mutex);
            g_quietWindowState.reset();
        }));

        //*g_quietWindowState = QuietWindowState::QuietWindowStateEnable::Enable();
        g_quietWindowState = QuietWindowState::turnOn();
        return g_activeTimer->TimeLeftInSeconds();
    }

    void QuietWindow::StopQuietWindow()
    {
        auto lock = std::scoped_lock(g_mutex);
        if (!g_activeTimer)
        {
            return;
        }

        // Turn off the quiet window no matter what
        auto signalStop = wil::scope_exit([]() {
            g_quietWindowState.reset();
            g_timerFinished = true;
        });

        // Detach and destruct the current time window
        std::unique_ptr<Timer> oldWindow = std::move(g_activeTimer);
        oldWindow->Cancel();
        Timer::Destroy(std::move(*oldWindow));

        g_activeTimer = nullptr;
    }

    bool QuietWindow::IsActive()
    {
        auto lock = std::scoped_lock(g_mutex);
        return g_activeTimer != nullptr;
    }

    int64_t QuietWindow::TimeLeftInSeconds()
    {
        auto lock = std::scoped_lock(g_mutex);
        if (!g_activeTimer)
        {
            return 0;
        }
        return g_activeTimer->TimeLeftInSeconds();
    }
}
