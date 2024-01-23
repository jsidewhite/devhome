#pragma once

#include <wil/resource.h>

namespace QuietState
{
    void TurnOn();
    void TurnOff();
    void IsOn();

    using unique_quietwindowclose_call = wil::unique_call<decltype(&QuietState::TurnOff), QuietState::TurnOff>;
    
    unique_quietwindowclose_call turnOn();
}

