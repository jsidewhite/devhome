#include <pch.h>
#include <objbase.h>
#include <roregistrationapi.h>
#include <wrl\client.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\implements.h>
#include <wrl\module.h>
#include <memory>
#include <wil/result_macros.h>
#include <wil/win32_helpers.h>
#include <Class.h>

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

Event g_shutdownEvent;
RO_REGISTRATION_COOKIE g_registrationCookie = nullptr;

HRESULT ExeServerRegisterWinrtClasses(_In_ PCWSTR serverName)
{
    g_shutdownEvent.Attach(CreateEvent(nullptr, true, false, nullptr));
    if (!g_shutdownEvent.IsValid())
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    Module<OutOfProc>::Create([] { SetEvent(g_shutdownEvent.Get()); });

    HSTRING* activatableClasses;
    DWORD activatableClassCount;
    RETURN_IF_FAILED(RoGetServerActivatableClasses(HStringReference(serverName).Get(),
                                                   &activatableClasses,
                                                   &activatableClassCount));

    PFNGETACTIVATIONFACTORY callback = [](HSTRING name, IActivationFactory** factory) -> HRESULT {
        THROW_HR_IF(E_UNEXPECTED, wil::compare_string_ordinal(WindowsGetStringRawBuffer(name, nullptr), L"zserver.Class", true) != 0);

        auto x = winrt::make<winrt::QuietBackgroundProcesses_ElevatedServer::factory_implementation::Class>();
        x.as<winrt::Windows::Foundation::IActivationFactory>();
        *factory = static_cast<IActivationFactory*>(winrt::detach_abi(x));
        return S_OK;
    };

    PFNGETACTIVATIONFACTORY callbacks[1] = { callback };

    RETURN_IF_FAILED(RoRegisterActivationFactories(activatableClasses, callbacks, activatableClassCount, &g_registrationCookie));

    return S_OK;
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

    PCWSTR serverName = wargv + wcslen(serverNamePrefix);
    Microsoft::WRL::Wrappers::RoInitializeWrapper roInit(RO_INIT_MULTITHREADED);

    THROW_IF_FAILED(ExeServerRegisterWinrtClasses(serverName));

    WaitForSingleObject(g_shutdownEvent.Get(), INFINITE);

    if (g_registrationCookie)
    {
        RoRevokeActivationFactories(g_registrationCookie);
        g_registrationCookie = nullptr;
    }

    return 0;
}
CATCH_RETURN();
