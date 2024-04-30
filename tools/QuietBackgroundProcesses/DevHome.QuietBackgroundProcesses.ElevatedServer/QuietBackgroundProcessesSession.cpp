// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

#include <chrono>
#include <memory>
#include <mutex>

#include <TraceLoggingProvider.h>

#include <wrl/client.h>
#include <wrl/implements.h>
#include <wrl/module.h>

#include <wil/resource.h>
#include <wil/result_macros.h>
#include <wil/win32_helpers.h>
#include <wil/winrt.h>
#include <wil/Tracelogging.h>

#include "TimedQuietSession.h"

#include "DevHome.QuietBackgroundProcesses.h"

TRACELOGGING_DEFINE_PROVIDER(
    g_hTelemetryProvider,
    "Microsoft.Windows.DevHome",
    (0x2e74ff65, 0xbbda, 0x5e80, 0x4c, 0x0a, 0xbd, 0x83, 0x20, 0xd4, 0x22, 0x3b),
    TraceLoggingOptionMicrosoftTelemetry());

class SnapFlyoutTelemetry : public wil::TraceLoggingProvider
{
    IMPLEMENT_TRACELOGGING_CLASS_WITH_MICROSOFT_TELEMETRY(SnapFlyoutTelemetry,
                                                          "Microsoft.Windows.Shell.ExplorerExtensions.SnapFlyout",
                                                          /* dc731193-9640-46e7-a49c-6eccd03d94c2*/
                                                          (0xdc731193, 0x9640, 0x46e7, 0xa4, 0x9c, 0x6e, 0xcc, 0xd0, 0x3d, 0x94, 0xc2));

public:
    DEFINE_COMPLIANT_MEASURES_EVENT_PARAM2(SnapZoneActivated, PDT_ProductAndServicePerformance | PDT_ProductAndServiceUsage, PCWSTR, zoneName, PCWSTR, layoutName);

    DEFINE_COMPLIANT_MEASURES_EVENT_PARAM1(SnapSuggestionActivated, PDT_ProductAndServicePerformance | PDT_ProductAndServiceUsage, PCWSTR, layoutName);

    DEFINE_COMPLIANT_MEASURES_EVENT_PARAM1(SnapFlyoutShown, PDT_ProductAndServicePerformance | PDT_ProductAndServiceUsage, bool, invokedbyHotkey);

    DEFINE_TRACELOGGING_EVENT_STRING(TraceMessage, Message);

    DEFINE_COMPLIANT_MEASURES_EVENT_PARAM1(UnexpectedVisibilityState, PDT_ProductAndServicePerformance, unsigned, state);
};




[uuid(5526aed1-f6e5-5896-cbf0-27d9f59b6be7)]
class DesktopAppXProvider : public wil::TraceLoggingProvider
{
    // {5526aed1-f6e5-5896-cbf0-27d9f59b6be7}
    IMPLEMENT_TRACELOGGING_CLASS(DesktopAppXProvider, "Microsoft.Windows.ApplicationModel.DesktopAppx", (0x5526aed1, 0xf6e5, 0x5896, 0xcb, 0xf0, 0x27, 0xd9, 0xf5, 0x9b, 0x6b, 0xe7));

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
};

constexpr auto DEFAULT_QUIET_DURATION = std::chrono::hours(2);

std::mutex g_mutex;
std::unique_ptr<TimedQuietSession> g_activeTimer;

namespace ABI::DevHome::QuietBackgroundProcesses
{
    class QuietBackgroundProcessesSession :
        public Microsoft::WRL::RuntimeClass<
            Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRt>,
            IQuietBackgroundProcessesSession,
            Microsoft::WRL::FtmBase>
    {
        InspectableClass(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSession, BaseTrust);

    public:
        STDMETHODIMP RuntimeClassInitialize() noexcept
        {
            return S_OK;
        }

        // IQuietBackgroundProcessesSession
        STDMETHODIMP Start(__int64* result) noexcept override try
        {
            auto lock = std::scoped_lock(g_mutex);

            // Stop and discard the previous timer
            if (g_activeTimer)
            {
                g_activeTimer->Cancel(nullptr);
            }

            std::chrono::seconds duration = DEFAULT_QUIET_DURATION;
            if (auto durationOverride = try_get_registry_value_dword(HKEY_LOCAL_MACHINE, LR"(Software\Microsoft\Windows\CurrentVersion\DevHome\QuietBackgroundProcesses)", L"Duration"))
            {
                duration = std::chrono::seconds(durationOverride.value());
            }

            // Start timer
            g_activeTimer.reset(new TimedQuietSession(duration));

            // Return duration for showing countdown
            *result = g_activeTimer->TimeLeftInSeconds();
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP Stop(ABI::DevHome::QuietBackgroundProcesses::IProcessPerformanceTable** result) noexcept override
        try
        {
            auto lock = std::scoped_lock(g_mutex);
            *result = nullptr;

            // Turn off quiet mode and cancel timer
            if (g_activeTimer)
            {
                g_activeTimer->Cancel(result);
                g_activeTimer.reset();
            }

            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_IsActive(::boolean* value) noexcept override try
        {
            auto lock = std::scoped_lock(g_mutex);
            *value = false;
            if (g_activeTimer)
            {
                *value = g_activeTimer->IsActive();
            }
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_TimeLeftInSeconds(__int64* value) noexcept override try
        {
            auto lock = std::scoped_lock(g_mutex);
            *value = 0;
            if (g_activeTimer)
            {
                *value = g_activeTimer->TimeLeftInSeconds();
            }
            return S_OK;
        }
        CATCH_RETURN()
    };

    class QuietBackgroundProcessesSessionStatics WrlFinal :
        public Microsoft::WRL::AgileActivationFactory<
            Microsoft::WRL::Implements<IQuietBackgroundProcessesSessionStatics>>
    {
        InspectableClassStatic(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSession, BaseTrust);

    public:
        STDMETHODIMP ActivateInstance(_COM_Outptr_ IInspectable**) noexcept
        {
            // Disallow activation - must use GetSingleton()
            return E_NOTIMPL;
        }

        // IQuietBackgroundProcessesSessionStatics
        STDMETHODIMP GetSingleton(_COM_Outptr_ IQuietBackgroundProcessesSession** session) noexcept override try
        {
            // Instanced objects are the only feasible way to manage a COM singleton without keeping a strong
            // handle to the server - which keeps it alive.  (IWeakReference keeps a strong handle to the server!)
            // An 'instance' can be thought of as a 'handle' to 'the singleton' backend.
            THROW_IF_FAILED(Microsoft::WRL::MakeAndInitialize<QuietBackgroundProcessesSession>(session));
            return S_OK;
        }
        CATCH_RETURN()
    };

    ActivatableClassWithFactory(QuietBackgroundProcessesSession, QuietBackgroundProcessesSessionStatics);
}
