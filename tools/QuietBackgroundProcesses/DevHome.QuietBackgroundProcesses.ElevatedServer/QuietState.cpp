#include "pch.h"

#include <mutex>

#include "QuietState.h"

namespace QuietState
{
    std::mutex g_mutex;

    namespace details
    {
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

    unique_quietwindowclose_call TurnOn()
    {
        details::TurnOn();
        return {};
    }
}
