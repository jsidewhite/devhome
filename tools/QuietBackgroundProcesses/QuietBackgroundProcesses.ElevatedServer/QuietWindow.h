#pragma once
#include "QuietWindow.g.h"

namespace winrt::QuietBackgroundProcesses_ElevatedServer::implementation
{
    struct QuietWindow : QuietWindowT<QuietWindow>
    {
        QuietWindow() = default;

        static int64_t StartQuietWindow();
        static void StopQuietWindow();
        static bool IsActive();
        static int64_t TimeLeftInSeconds();
    };
}
namespace winrt::QuietBackgroundProcesses_ElevatedServer::factory_implementation
{
    struct QuietWindow : QuietWindowT<QuietWindow, implementation::QuietWindow>
    {
    };
}
