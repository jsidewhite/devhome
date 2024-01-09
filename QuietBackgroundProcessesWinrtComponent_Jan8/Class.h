#pragma once

#include "Class.g.h"

namespace winrt::QuietBackgroundProcessesWinrtComponent_Jan8::implementation
{
    struct Class : ClassT<Class>
    {
        Class() = default;

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::QuietBackgroundProcessesWinrtComponent_Jan8::factory_implementation
{
    struct Class : ClassT<Class, implementation::Class>
    {
    };
}
