// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

#include <pch.h>

#include <mutex>

#include <objbase.h>
#include <roregistrationapi.h>
#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/implements.h>
#include <wrl/module.h>
#include <memory>
#include <wil/result_macros.h>
#include <wil/token_helpers.h>
#include <wil/win32_helpers.h>
#include <DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesManager.h>
#include "QuietState.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

std::mutex g_finishMutex;
std::condition_variable g_finishCondition;
bool g_lastInstanceOfTheModuleObjectIsReleased;

bool IsTokenElevated(HANDLE token)
{
    auto mandatoryLabel = wil::get_token_information<TOKEN_MANDATORY_LABEL>(token);
    LONG levelRid = static_cast<SID*>(mandatoryLabel->Label.Sid)->SubAuthority[0];
    return levelRid == SECURITY_MANDATORY_HIGH_RID;
}

wil::unique_ro_registration_cookie RegisterWinrtClasses(_In_ PCWSTR serverName)
{
    Module<OutOfProc>::Create([] {
        // The last instance object of the module is released
        {
            auto lock = std::unique_lock<std::mutex>(g_finishMutex);
            g_lastInstanceOfTheModuleObjectIsReleased = true;
        }
        g_finishCondition.notify_one();
    });

    HSTRING* activatableClasses;
    DWORD activatableClassCount;
    THROW_IF_FAILED(RoGetServerActivatableClasses(HStringReference(serverName).Get(), &activatableClasses, &activatableClassCount));

    PFNGETACTIVATIONFACTORY callback = [](HSTRING name, IActivationFactory** factory) -> HRESULT {
        RETURN_HR_IF(E_UNEXPECTED, wil::compare_string_ordinal(WindowsGetStringRawBuffer(name, nullptr), L"DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesManager", true) != 0);

        auto manager = winrt::make<winrt::DevHome::QuietBackgroundProcesses::factory_implementation::QuietBackgroundProcessesManager>();
        manager.as<winrt::Windows::Foundation::IActivationFactory>();
        *factory = static_cast<IActivationFactory*>(winrt::detach_abi(manager));
        return S_OK;
    };

    wil::unique_ro_registration_cookie registrationCookie;
    PFNGETACTIVATIONFACTORY callbacks[1] = { callback };
    THROW_IF_FAILED(RoRegisterActivationFactories(activatableClasses, callbacks, activatableClassCount, &registrationCookie));
    return registrationCookie;
}

//int _cdecl wmain(int argc, __in_ecount(argc) PWSTR wargv[])
int CALLBACK wWinMain(_In_ HINSTANCE, _In_ HINSTANCE, _In_ LPWSTR wargv, _In_ int)
try
{
    constexpr WCHAR serverNamePrefix[] = L"-ServerName:";
    if (_wcsnicmp(wargv, serverNamePrefix, wcslen(serverNamePrefix)) != 0)
    {
        return E_UNEXPECTED;
    }

    if (!IsTokenElevated(GetCurrentProcessToken()))
    {
        return E_ACCESSDENIED;
    }

    // To be safe, force quiet mode off to begin the proceedings in case we leaked the machine state previously
    QuietState::TurnOff();

    PCWSTR serverName = wargv + wcslen(serverNamePrefix);
    auto unique_rouninitialize_call = wil::RoInitialize();

    // Register WinRT activatable classes
    auto registrationCookie = RegisterWinrtClasses(serverName);

    // Wait for the module objects to be released and the timer threads to finish
    {
        auto lock = std::unique_lock<std::mutex>(g_finishMutex);

        // Wait for both events to complete
        g_finishCondition.wait(lock, [] {
            return g_lastInstanceOfTheModuleObjectIsReleased && winrt::DevHome::QuietBackgroundProcesses::implementation::QuietBackgroundProcessesManager::IsActive();
        });
    }

    // To be safe, force quiet mode off
    QuietState::TurnOff();

    return 0;
}
CATCH_RETURN();
