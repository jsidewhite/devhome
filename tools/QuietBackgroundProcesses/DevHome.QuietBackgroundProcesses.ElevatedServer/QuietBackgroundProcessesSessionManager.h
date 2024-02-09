// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

#pragma once

#include "QuietBackgroundProcessesSessionManager.g.h"

namespace winrt::DevHome::QuietBackgroundProcesses::implementation
{
    struct QuietBackgroundProcessesSessionManager : QuietBackgroundProcessesSessionManagerT<QuietBackgroundProcessesSessionManager>
    {
        QuietBackgroundProcessesSessionManager() = default;

        static winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession GetInstance();
        static winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession CreateInstance();
    };
}
namespace winrt::DevHome::QuietBackgroundProcesses::factory_implementation
{
    struct QuietBackgroundProcessesSessionManager : QuietBackgroundProcessesSessionManagerT<QuietBackgroundProcessesSessionManager, implementation::QuietBackgroundProcessesSessionManager>
    {
    };
}
