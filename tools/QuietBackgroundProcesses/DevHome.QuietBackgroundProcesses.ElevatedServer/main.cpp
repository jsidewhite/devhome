// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

#include <pch.h>

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID DECLSPEC_SELECTANY name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

// 33a0a1a0-b89c-44af-ba17-c828cea010c2
#define INITGUID
DEFINE_GUID(CLSID_DevHomeQuietBackgroundProcessesElevatedServerRunningProbe, 0x33a0a1a0, 0xb89c, 0x44af, 0xba, 0x17, 0xc8, 0x28, 0xce, 0xa0, 0x10, 0xc2);


#include <functional>
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

#include <objbase.h>
#include <roregistrationapi.h>
#include <shellapi.h>

// #include <DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.h>
#include "QuietBackgroundProcessesSession.h"
#include "QuietState.h"

#include "classfactory.h"

using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

using unique_hstring_array_ptr = wil::unique_any_array_ptr<HSTRING, wil::cotaskmem_deleter, wil::function_deleter<decltype(::WindowsDeleteString), ::WindowsDeleteString>>;

std::mutex g_finishMutex;
std::condition_variable g_finishCondition;
bool g_lastInstanceOfTheModuleObjectIsReleased;

bool IsTokenElevated(HANDLE token)
{
    auto mandatoryLabel = wil::get_token_information<TOKEN_MANDATORY_LABEL>(token);
    LONG levelRid = static_cast<SID*>(mandatoryLabel->Label.Sid)->SubAuthority[0];
    return levelRid == SECURITY_MANDATORY_HIGH_RID;
}

wil::unique_ro_registration_cookie RegisterWinrtClasses(_In_ PCWSTR serverName, std::function<void()> objectsReleasedCallback)
{
    Module<OutOfProc>::Create(objectsReleasedCallback);

    // Get module classes
    unique_hstring_array_ptr classes;
    THROW_IF_FAILED(RoGetServerActivatableClasses(HStringReference(serverName).Get(), &classes, reinterpret_cast<DWORD*>(classes.size_address())));

    // Creation callback
    PFNGETACTIVATIONFACTORY callback = [](HSTRING name, IActivationFactory** factory) -> HRESULT {
        //if (wil::compare_string_ordinal(serverName, L"DevHome.QuietBackgroundProcesses.Server", true) == 0)
        {
            if (wil::compare_string_ordinal(WindowsGetStringRawBuffer(name, nullptr), L"DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager", true) == 0)
            {
                auto manager = winrt::make<winrt::DevHome::QuietBackgroundProcesses::factory_implementation::QuietBackgroundProcessesSession>();
                manager.as<winrt::Windows::Foundation::IActivationFactory>();
                *factory = static_cast<IActivationFactory*>(winrt::detach_abi(manager));
                return S_OK;
            }
        }

        //if (wil::compare_string_ordinal(serverName, L"DevHome.QuietBackgroundProcesses.ElevatedServer", true) == 0)
        {
            if (wil::compare_string_ordinal(WindowsGetStringRawBuffer(name, nullptr), L"DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession", true) == 0)
            {
                auto manager = winrt::make<winrt::DevHome::QuietBackgroundProcesses::factory_implementation::QuietBackgroundProcessesSession>();
                manager.as<winrt::Windows::Foundation::IActivationFactory>();
                *factory = static_cast<IActivationFactory*>(winrt::detach_abi(manager));
                return S_OK;
            }
        }

        
        RETURN_HR(E_UNEXPECTED);
    };

    // Register
    wil::unique_ro_registration_cookie registrationCookie;
    PFNGETACTIVATIONFACTORY callbacks[1] = { callback };
    THROW_IF_FAILED(RoRegisterActivationFactories(classes.get(), callbacks, static_cast<UINT32>(classes.size()), &registrationCookie));
    return registrationCookie;
}

void SelfElevate(std::optional<std::wstring> const& arguments)
{
    wchar_t szPath[MAX_PATH];
    if (!GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
    {
        // MessageBox(hwnd, L"Couldn't find module file name", L"Couldn't find module file name", 0);
        THROW_LAST_ERROR();
    }

    SHELLEXECUTEINFO sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    sei.lpFile = szPath;
    sei.lpParameters = arguments.value().c_str();
    sei.hwnd = NULL;
    sei.nShow = SW_NORMAL;

    if (!ShellExecuteEx(&sei))
    {
        // Elevated instance launched; close this one.
        // _exit(1);
        //THROW_HR(E_APPLICATION_EXITING);

        THROW_LAST_ERROR();
    }

    
}


//int _cdecl wmain(int argc, __in_ecount(argc) PWSTR wargv[])
int CALLBACK wWinMain(_In_ HINSTANCE, _In_ HINSTANCE, _In_ LPWSTR wargv, _In_ int)
try
{
    // -ServerName:DevHome.QuietBackgroundProcesses.ElevatedServer
    constexpr WCHAR serverNamePrefix[] = L"-ServerName:";
    if (_wcsnicmp(wargv, serverNamePrefix, wcslen(serverNamePrefix)) != 0)
    {
        return E_UNEXPECTED;
    }
    
    auto unique_rouninitialize_call = wil::RoInitialize();

    if (!IsTokenElevated(GetCurrentProcessToken()))
    {
        /*
            while (!IsDebuggerPresent())
        {
            Sleep(100);
        };
        DebugBreak();
        //THROW_IF_FAILED(::CoRegisterClassObject(CLSID_DevHomeQuietBackgroundProcessesElevatedServerRunningProbe, pIFactory, CLSCTX_LOCAL_SERVER, REGCLS_SUSPENDED | REGCLS_MULTIPLEUSE, &dwWimSandboxRegistered));
        wil::com_ptr<IClassFactory> pFactory = new SimpleClassFactory2();
        DWORD isRegistered = 0;
        THROW_IF_FAILED(::CoRegisterClassObject(CLSID_DevHomeQuietBackgroundProcessesElevatedServerRunningProbe, pFactory.get(), CLSCTX_LOCAL_SERVER, REGCLS_MULTIPLEUSE, &isRegistered));
        */


        //SelfElevate(wargv);
        //return E_ACCESSDENIED;
        // Sleep(600000);
        // return 0;
    }
    else
    {

    }

        while (!IsDebuggerPresent())
    {
        Sleep(100);
    };
    DebugBreak();

    auto serverName = std::wstring{} + (wargv + wcslen(serverNamePrefix));

    // To be safe, force quiet mode off to begin the proceedings in case we leaked the machine state previously
    QuietState::TurnOff();

    // PCWSTR serverName = wargv + wcslen(serverNamePrefix);
//    auto unique_rouninitialize_call = wil::RoInitialize();

    // Register WinRT activatable classes
    auto registrationCookie = RegisterWinrtClasses(serverName.c_str(), [] {
        // The last instance object of the module is released
        {
            auto lock = std::unique_lock<std::mutex>(g_finishMutex);
            g_lastInstanceOfTheModuleObjectIsReleased = true;
        }
        g_finishCondition.notify_one();
    });


    // Wait for the module objects to be released and the timer threads to finish
    {
        auto lock = std::unique_lock<std::mutex>(g_finishMutex);

        // Wait for both events to complete
        g_finishCondition.wait(lock, [] {
            // return g_lastInstanceOfTheModuleObjectIsReleased && winrt::DevHome::QuietBackgroundProcesses::implementation::QuietBackgroundProcessesSession::IsActive();
            // todo:jw
            return g_lastInstanceOfTheModuleObjectIsReleased;
        });
    }

    // To be safe, force quiet mode off
    QuietState::TurnOff();

    return 0;
}
CATCH_RETURN();
