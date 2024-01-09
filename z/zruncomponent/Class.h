#pragma once

#include "Class.g.h"

namespace winrt::zruncomponent::implementation
{
    struct Class : ClassT<Class>
    {
        Class() = default;

        int32_t MyProperty();
        void MyProperty(int32_t value);
    };
}

namespace winrt::zruncomponent::factory_implementation
{
    struct Class : ClassT<Class, implementation::Class>
    {
    };
}
