#pragma once
#include "QuietBackgroundProcessesManager.g.h"

namespace winrt::QuietBackgroundProcesses_ElevatedServer::implementation
{
    struct QuietBackgroundProcessesManager : QuietBackgroundProcessesManagerT<QuietBackgroundProcessesManager>
    {
        QuietBackgroundProcessesManager() = default;

        static int64_t StartQuietWindow();
        static void StopQuietWindow();
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
