// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <wil/resource.h>
#include "DevHome.QuietBackgroundProcesses.h"

wil::com_ptr<ABI::DevHome::QuietBackgroundProcesses::IPerformanceRecorderEngine> MakePerformanceRecorderEngine();
