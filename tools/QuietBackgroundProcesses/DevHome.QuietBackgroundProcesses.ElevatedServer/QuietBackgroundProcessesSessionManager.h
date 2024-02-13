#pragma once
#include "QuietBackgroundProcessesSessionManager.g.h"

namespace winrt::DevHome::QuietBackgroundProcesses::implementation
{
    struct QuietBackgroundProcessesSessionManager : QuietBackgroundProcessesSessionManagerT<QuietBackgroundProcessesSessionManager>
    {
        QuietBackgroundProcessesSessionManager() = default;

        static winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession GetSession();
        static winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession TryGetSession();
        static winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesThingy TryGetThingy();

        static int32_t GetInt();
    };
}
namespace winrt::DevHome::QuietBackgroundProcesses::factory_implementation
{
    struct QuietBackgroundProcessesSessionManager : QuietBackgroundProcessesSessionManagerT<QuietBackgroundProcessesSessionManager, implementation::QuietBackgroundProcessesSessionManager>
    {
    };
}
