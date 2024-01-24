#include "pch.h"
#include "Timer.h"

std::vector<Timer> g_discardedTimers;
std::mutex g_discardMutex;
std::thread g_discardThread;

static void CleanupDiscardedTimers()
{
    std::vector<Timer> discardedTimers;
    {
        auto lock = std::scoped_lock(g_discardMutex);
        discardedTimers = std::move(g_discardedTimers);
    }

    // Destruct time window on sepearate thread because its destructor may take time to end (the std::future member is blocking)
    auto th = std::thread([discardedTimers = std::move(discardedTimers)]()
    {
        discardedTimers.empty(); // This will block until all timers have woken up and cancel themselves
    });
    th.detach();
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

    // Destruct time window on sepearate thread because its destructor may take time to end (the std::future member is blocking)
    auto th = std::thread([timer = std::move(timer)]() {
        timer.reset();
    });
    th.detach();
}
