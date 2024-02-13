#pragma once
#include "QuietBackgroundProcessesThingy.g.h"

namespace winrt::DevHome::QuietBackgroundProcesses::implementation
{
    struct QuietBackgroundProcessesThingy : QuietBackgroundProcessesThingyT<QuietBackgroundProcessesThingy>
    {
        QuietBackgroundProcessesThingy() = default;

        int32_t Thingy();
    };
}
namespace winrt::DevHome::QuietBackgroundProcesses::factory_implementation
{
    struct QuietBackgroundProcessesThingy : QuietBackgroundProcessesThingyT<QuietBackgroundProcessesThingy, implementation::QuietBackgroundProcessesThingy>
    {
    };
}
