#include "pch.h"

#include <mutex>

#include <QuietBackgroundProcesses.h>
#include "QuietState.h"

namespace QuietState
{
    std::mutex g_mutex;

    namespace details
    {
        void TurnOff()
        {
            auto lock = std::scoped_lock(g_mutex);
            DisableQuietBackgroundProcesses();
        }
    }

    unique_quietwindowclose_call TurnOn()
    {
        auto lock = std::scoped_lock(g_mutex);
        EnableQuietBackgroundProcesses();
        return {};
    }
}
