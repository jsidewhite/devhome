// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "TimedQuietSession.h"

std::mutex g_discardMutex;
std::thread g_discardThread;

std::thread TimedQuietSession::GetDiscardThread()
{
    auto lock = std::scoped_lock(g_discardMutex);
    return std::move(g_discardThread);
}

void TimedQuietSession::Discard(std::unique_ptr<TimedQuietSession> quietSession)
{
    if (!quietSession)
    {
        return;
    }
    quietSession->m_timer.Cancel();

    auto lock = std::scoped_lock(g_discardMutex);

    // Destruct time window on separate thread because its destructor may take time to end (the std::future member is blocking)
    //
    // (Make a new discard thread and chain the existing one to it)
    g_discardThread = std::thread([quietSession = std::move(quietSession), previousThread = std::move(g_discardThread)]() mutable {
        // Delete the timer (blocking)
        quietSession.reset();

        // Finish previous discard thread if there was one
        if (previousThread.joinable())
        {
            previousThread.join();
        }
    });
}
