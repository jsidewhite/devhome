#pragma once

#include <wil/resource.h>

namespace QuietState
{
    namespace details
    {
        void TurnOff();
    }

    using unique_quietwindowclose_call = wil::unique_call<decltype(&details::TurnOff), details::TurnOff>;
    
    unique_quietwindowclose_call TurnOn();
}

