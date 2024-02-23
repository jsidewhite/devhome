// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Timer.h"

std::mutex g_discardMutex;
std::thread g_discardThread;

std::thread Timer::GetDiscardThread()
{
    auto lock = std::scoped_lock(g_discardMutex);
    return std::move(g_discardThread);
}

void Timer::Discard(std::unique_ptr<Timer> timer)
{
    if (!timer)
    {
        return;
    }
    timer->Cancel();

    auto lock = std::scoped_lock(g_discardMutex);

    // Destruct time window on sepearate thread because its destructor may take time to end (the std::future member is blocking)
    //
    // (Make a new discard thread and chain the existing one to it)
    g_discardThread = std::thread([timer = std::move(timer), previousThread = std::move(g_discardThread)]() mutable {
        // Delete the timer (blocking)
        timer.reset();

        // Finish previous discard thread if there was one
        if (previousThread.joinable())
        {
            previousThread.join();
        }
    });
}
