#include "pch.h"
#include "Timer.h"

std::vector<Timer> g_discardedTimers;
std::mutex g_discardMutex;

static void CleanupDiscardedTimers()
{
    auto lock = std::scoped_lock(g_discardMutex);
    for (auto& timer : g_discardedTimers)
    {
        // Destruct time window on sepearate thread because its destructor may take time to end (the std::future member is blocking)
        auto th = std::thread([discardedTimers = std::move(g_discardedTimers)]() {});
        th.detach();
    }
}

void Timer::Discard(std::unique_ptr<Timer> timer)
{
    if (!timer)
    {
        return;
    }
    timer->Cancel();

    // Put on the discard pile
    {
        auto lock = std::scoped_lock(g_discardMutex);
        g_discardedTimers.push_back(std::move(*timer));
    }

    // Clear the discard pile

}
