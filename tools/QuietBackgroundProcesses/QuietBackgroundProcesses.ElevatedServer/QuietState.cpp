#include "pch.h"

#include <mutex>

#include "QuietState.h"

namespace QuietState
{
    std::mutex g_mutex;

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



    unique_quietwindowclose_call turnOn()
    {
        TurnOn();
        return {};
    }
}
