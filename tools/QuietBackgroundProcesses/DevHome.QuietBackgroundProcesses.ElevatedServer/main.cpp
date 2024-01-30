// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

#include <pch.h>
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

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;


Event g_shutdownEvent;
RO_REGISTRATION_COOKIE g_registrationCookie = nullptr;

bool IsTokenElevated(HANDLE token)
{
    auto mandatoryLabel = wil::get_token_information<TOKEN_MANDATORY_LABEL>(token);
    LONG levelRid = static_cast<SID*>(mandatoryLabel->Label.Sid)->SubAuthority[0];
    return levelRid == SECURITY_MANDATORY_HIGH_RID;
}

void ExeServerRegisterWinrtClasses(_In_ PCWSTR serverName)
{
    g_shutdownEvent.Attach(CreateEvent(nullptr, true, false, nullptr));
    THROW_LAST_ERROR_IF(!g_shutdownEvent.IsValid());

    Module<OutOfProc>::Create([] { SetEvent(g_shutdownEvent.Get()); });

    HSTRING* activatableClasses;
    DWORD activatableClassCount;
    THROW_IF_FAILED(RoGetServerActivatableClasses(HStringReference(serverName).Get(),
                                                   &activatableClasses,
                                                   &activatableClassCount));

    PFNGETACTIVATIONFACTORY callback = [](HSTRING name, IActivationFactory** factory) -> HRESULT {
        RETURN_HR_IF(E_UNEXPECTED, wil::compare_string_ordinal(WindowsGetStringRawBuffer(name, nullptr), L"DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesManager", true) != 0);

        auto x = winrt::make<winrt::DevHome::QuietBackgroundProcesses::factory_implementation::QuietBackgroundProcessesManager>();
        x.as<winrt::Windows::Foundation::IActivationFactory>();
        *factory = static_cast<IActivationFactory*>(winrt::detach_abi(x));
        return S_OK;
    };

    PFNGETACTIVATIONFACTORY callbacks[1] = { callback };

    THROW_IF_FAILED(RoRegisterActivationFactories(activatableClasses, callbacks, activatableClassCount, &g_registrationCookie));
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

    PCWSTR serverName = wargv + wcslen(serverNamePrefix);
    Microsoft::WRL::Wrappers::RoInitializeWrapper roInit(RO_INIT_MULTITHREADED);

    ExeServerRegisterWinrtClasses(serverName);

    WaitForSingleObject(g_shutdownEvent.Get(), INFINITE);

    if (g_registrationCookie)
    {
        RoRevokeActivationFactories(g_registrationCookie);
        g_registrationCookie = nullptr;
    }

    return 0;
}
CATCH_RETURN();
