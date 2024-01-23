#pragma once

#include <wil/resource.h>

namespace QuietWindowState
{
    void TurnOn();
    void TurnOff();
    void IsOn();

    using unique_quietwindowclose_call = wil::unique_call<decltype(&QuietWindowState::TurnOff), QuietWindowState::TurnOff>;
    
    unique_quietwindowclose_call turnOn();
}

