#pragma once
#include "QuietWindow.g.h"

namespace winrt::QuietBackgroundProcesses_ElevatedServer::implementation
{
    struct QuietWindow : QuietWindowT<QuietWindow>
    {
        QuietWindow() = default;

        static bool IsActive();
        static int32_t TimeLeftInSeconds();
    };
}
namespace winrt::QuietBackgroundProcesses_ElevatedServer::factory_implementation
{
    struct QuietWindow : QuietWindowT<QuietWindow, implementation::QuietWindow>
    {
    };
}
