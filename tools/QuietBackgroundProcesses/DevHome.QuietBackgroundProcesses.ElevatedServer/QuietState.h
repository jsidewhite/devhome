// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

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

