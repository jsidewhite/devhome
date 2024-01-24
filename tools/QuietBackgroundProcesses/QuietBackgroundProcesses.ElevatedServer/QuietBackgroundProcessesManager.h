#pragma once
#include "QuietBackgroundProcessesManager.g.h"

namespace winrt::QuietBackgroundProcesses_ElevatedServer::implementation
{
    struct QuietBackgroundProcessesManager : QuietBackgroundProcessesManagerT<QuietBackgroundProcessesManager>
    {
        QuietBackgroundProcessesManager() = default;

        static int64_t Start();
        static void Stop();
        static bool IsActive();
        static int64_t TimeLeftInSeconds();
    };
}
namespace winrt::QuietBackgroundProcesses_ElevatedServer::factory_implementation
{
    struct QuietBackgroundProcessesManager : QuietBackgroundProcessesManagerT<QuietBackgroundProcessesManager, implementation::QuietBackgroundProcessesManager>
    {
    };
}
