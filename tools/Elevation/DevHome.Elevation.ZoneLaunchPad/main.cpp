// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

#include <filesystem>
#include <memory>
#include <mutex>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/implements.h>
#include <wrl/module.h>
#include <wil/com.h>
#include <wil/result_macros.h>
#include <wil/token_helpers.h>
#include <wil/win32_helpers.h>
#include <wil/winrt.h>

#include <objbase.h>
#include <roregistrationapi.h>

#include "DevHome.Elevation.h"
#include "Utility.h"

int __stdcall wWinMain(HINSTANCE, HINSTANCE, LPWSTR wargv, int wargc) noexcept try
{
    if (wargc < 1)
    {
        THROW_HR(E_INVALIDARG);
    }

    // Parse the target Zone to be launched
    //auto zoneName = wargv[1];
    auto zoneName = wargv;

    if (wil::compare_string_ordinal(zoneName, L"ZoneA", true) != 0)
    {
        THROW_HR(E_INVALIDARG);
    }

    // Make sure we're already elevated instance
    if (!IsTokenElevated(GetCurrentProcessToken()))
    {
        THROW_HR(E_INVALIDARG);
    }

    WaitForDebuggerIfPresent();

    auto unique_rouninitialize_call = wil::RoInitialize();

    //wil::CoCreateInstance<ABI::DevHome::Elevation::ZoneConnectionManager>(CLSID_ZoneA, CLSCTX_LOCAL_SERVER);
    auto zoneConnectionManager = wil::GetActivationFactory<ABI::DevHome::Elevation::IZoneConnectionManagerStatics>(L"DevHome.Elevation.ZoneConnectionManager");

    std::mutex mutex;
    bool comFinished{};
    std::condition_variable finishCondition;

#pragma warning(push)
#pragma warning(disable : 4324) // Avoid WRL alignment warning

    // Register WRL callback when all objects are destroyed
    auto& module = Microsoft::WRL::Module<Microsoft::WRL::OutOfProc>::Create([&] {
        // The last instance object of the module is released
        {
            auto lock = std::unique_lock<std::mutex>(mutex);
            comFinished = true;
        }
        finishCondition.notify_one();
    });

#pragma warning(pop)

    // Register WinRT activatable classes
    module.RegisterObjects();
    auto unique_wrl_registration_cookie = wil::scope_exit([&module]() {
        module.UnregisterObjects();
    });

    // Tell the unelevated server instance that we've registered our winrt classes with COM (so it can terminate)
    wil::unique_event elevatedServerRunningEvent;
    elevatedServerRunningEvent.open(ELEVATED_SERVER_STARTED_EVENT_NAME);
    elevatedServerRunningEvent.SetEvent();

    // Wait for all server references to release (implicitly also waiting for timers to finish via CoAddRefServerProcess)
    auto lock = std::unique_lock<std::mutex>(mutex);

    finishCondition.wait(lock, [&] {
        return comFinished;
    });

    // Safety
    QuietState::TurnOff();

    return 0;
}
CATCH_RETURN()
