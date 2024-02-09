// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

#pragma once

#include "QuietBackgroundProcessesSession.g.h"

namespace winrt::DevHome::QuietBackgroundProcesses::implementation
{
    struct QuietBackgroundProcessesSession : QuietBackgroundProcessesSessionT<QuietBackgroundProcessesSession>
    {
        QuietBackgroundProcessesSession() = default;

        static int64_t Start();
        static void Stop();
        static bool IsActive();
        static int64_t TimeLeftInSeconds();
        static uint64_t GetProcessCpuUsage(uint32_t processId);
    };
}
namespace winrt::DevHome::QuietBackgroundProcesses::factory_implementation
{
    struct QuietBackgroundProcessesSession : QuietBackgroundProcessesSessionT<QuietBackgroundProcessesSession, implementation::QuietBackgroundProcessesSession>
    {
    };
}
