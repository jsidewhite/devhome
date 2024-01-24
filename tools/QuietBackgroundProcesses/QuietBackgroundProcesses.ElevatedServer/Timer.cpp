#include "pch.h"
#include "Timer.h"

std::vector<Timer> g_discardedTimers;

static void CleanupDiscardedTimers()
{
    // g_discardedTimers
}

void Timer::Discard(Timer* timer)
{
    ::CleanupDiscardedTimers();

    if (!timer)
    {
        return;
    }
    timer->Cancel();
    g_discardedTimers.push_back(std::move(*timer));
}
