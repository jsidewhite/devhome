// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

#pragma once

#include "QuietBackgroundProcessesSession.g.h"

namespace winrt::DevHome::QuietBackgroundProcesses::implementation
{
    struct QuietBackgroundProcessesSession : QuietBackgroundProcessesSessionT<QuietBackgroundProcessesSession>
    {
        QuietBackgroundProcessesSession() = default;

        static winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession GetSingleton();
        int64_t Start();
        void Stop();
        bool IsActive();
        int64_t TimeLeftInSeconds();
    };
}
namespace winrt::DevHome::QuietBackgroundProcesses::factory_implementation
{
    struct QuietBackgroundProcessesSession : QuietBackgroundProcessesSessionT<QuietBackgroundProcessesSession, implementation::QuietBackgroundProcessesSession>
    {
    };
}
