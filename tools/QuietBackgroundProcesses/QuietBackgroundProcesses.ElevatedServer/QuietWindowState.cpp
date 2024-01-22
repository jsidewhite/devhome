#include "pch.h"

#include <mutex>

#include "QuietWindowState.h"

namespace QuietWindowState
{
    std::atomic<std::mutex> g_mutex;

    void TurnOn()
    {
        auto lock = std::scoped_lock(g_mutex);
    }

    void TurnOff()
    {
        auto lock = std::scoped_lock(g_mutex);
    }

    void IsOn()
    {

    }
}
