// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

//#include <TraceLogging.h>
#include <TraceLoggingProvider.h>
#include <MicrosoftTelemetry.h>

#define __WIL_TRACELOGGING_CONFIG_H
#include <wil/Tracelogging.h>

// [uuid(2e74ff65-bbda-5e80-4c0a-bd8320d4223b)]
class DevHomeTelemetryProvider : public wil::TraceLoggingProvider
{
    IMPLEMENT_TRACELOGGING_CLASS(DevHomeTelemetryProvider, "Microsoft.Windows.DevHome", (0x2e74ff65, 0xbbda, 0x5e80, 0x4c, 0x0a, 0xbd, 0x83, 0x20, 0xd4, 0x22, 0x3b));

public:

    BEGIN_TRACELOGGING_ACTIVITY_CLASS(QuietBackgroundProcessesSession);
    DEFINE_ACTIVITY_START(uint64_t expectedDuration)
    {
        TraceLoggingClassWriteStart(
            QuietModeSession,
            TraceLoggingValue(expectedDuration, "ExpectedDuration"));
    }
    DEFINE_ACTIVITY_STOP(HRESULT hr, uint64_t actualDuration)
    {
        TraceLoggingClassWriteStop(
            QuietBackgroundProcessesSession,
            TraceLoggingValue(hr, "Hresult"),
            TraceLoggingValue(actualDuration, "ActualDuration"));
    }
    END_ACTIVITY_CLASS();
};
