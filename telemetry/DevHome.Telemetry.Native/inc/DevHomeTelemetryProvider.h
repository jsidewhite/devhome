// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

//#include <TraceLogging.h>
#include <TraceLoggingProvider.h>
#include <MicrosoftTelemetry.h>

#include <wil/Tracelogging.h>

TRACELOGGING_DEFINE_PROVIDER(
    g_hTelemetryProvider,
    "Microsoft.Windows.DevHome",
    (0x2e74ff65, 0xbbda, 0x5e80, 0x4c, 0x0a, 0xbd, 0x83, 0x20, 0xd4, 0x22, 0x3b),
    TraceLoggingOptionMicrosoftTelemetry());

// [uuid(2e74ff65-bbda-5e80-4c0a-bd8320d4223b)]
class DevHomeTelemetryProvider : public wil::TraceLoggingProvider
{
    IMPLEMENT_TRACELOGGING_CLASS(DevHomeTelemetryProvider, "Microsoft.Windows.DevHome", (0x2e74ff65, 0xbbda, 0x5e80, 0x4c, 0x0a, 0xbd, 0x83, 0x20, 0xd4, 0x22, 0x3b));

public:
    BEGIN_TRACELOGGING_ACTIVITY_CLASS(AllowServicing)
    DEFINE_ACTIVITY_START(int packageCount, BOOL terminateRunningApps)
    {
        TraceLoggingClassWriteStart(AllowServicing,
                                    TraceLoggingValue(packageCount, "packageCount"),
                                    TraceLoggingValue(terminateRunningApps, "terminateRunningApps"));
    }
    END_ACTIVITY_CLASS();

    DEFINE_EVENT_METHOD(JitvAllowServicingError)
    (PCWSTR * activeApps, int appCount)
    {
        if (activeApps != nullptr && appCount > 0)
        {
            for (int i = 0; i < appCount; i++)
            {
                TraceLoggingWrite(
                    TraceLoggingType::Provider(),
                    "JitvAllowServicingError",
                    TraceLoggingValue(activeApps[i] == nullptr ? L"NULL" : activeApps[i], "App"),
                    TraceLoggingKeyword(MICROSOFT_KEYWORD_TELEMETRY),
                    TraceLoggingOpcode(WINEVENT_OPCODE_INFO),
                    TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                    TelemetryPrivacyDataTag(PDT_ProductAndServiceUsage));
            }
        }
        else
        {
            TraceLoggingWrite(
                TraceLoggingType::Provider(),
                "JitvAllowServicingError",
                TraceLoggingValue(L"Invalid Data", "App"),
                TraceLoggingKeyword(MICROSOFT_KEYWORD_TELEMETRY),
                TraceLoggingOpcode(WINEVENT_OPCODE_INFO),
                TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                TelemetryPrivacyDataTag(PDT_ProductAndServiceUsage));
        }
    }

    BEGIN_TRACELOGGING_ACTIVITY_CLASS(ShutdownPackage)
    DEFINE_ACTIVITY_START(PCWSTR packageFullName, bool isUninstall)
    {
        TraceLoggingClassWriteStart(ShutdownPackage,
                                    TraceLoggingValue(packageFullName, "packageFullName"),
                                    TraceLoggingValue(isUninstall, "isUninstall"));
    }
    END_ACTIVITY_CLASS();





    BEGIN_TRACELOGGING_ACTIVITY_CLASS(QuietModeSession);
    DEFINE_ACTIVITY_START(UINT32 expectedDuration)
    {
        TraceLoggingClassWriteStart(QuietModeSession,
                                    TraceLoggingValue(expectedDuration, "ExpectedDuration"));
    }
    DEFINE_ACTIVITY_STOP(UINT32 actualDuration)
    {
        TraceLoggingClassWriteStop(QuietModeSession,
                                   TraceLoggingValue(actualDuration, "ActualDuration"));
    }
    END_ACTIVITY_CLASS();
};
