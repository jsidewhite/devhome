// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

#include "pch.h"

#include <mutex>

#include <QuietBackgroundProcesses.h>
#include "QuietState.h"

namespace QuietState
{
    std::mutex g_mutex;

    void TurnOff()
    {
        auto lock = std::scoped_lock(g_mutex);
        try
        {
            //DisableQuietBackgroundProcesses();
        }
        catch (...)
        {
        }
    }

    unique_quietwindowclose_call TurnOn()
    {
        auto lock = std::scoped_lock(g_mutex);
        try
        {

            //THROW_IF_FAILED(EnableQuietBackgroundProcesses());
        }
        catch (...)
        {
        }

        return {};
    }
}
