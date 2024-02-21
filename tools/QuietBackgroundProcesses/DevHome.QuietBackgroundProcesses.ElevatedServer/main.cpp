// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

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

#include "QuietState.h"
#include "Utility.h"

std::mutex g_finishMutex;
std::condition_variable g_finishCondition;
bool g_lastInstanceOfTheModuleObjectIsReleased;

static wil::unique_ro_registration_cookie RegisterWinrtClasses(_In_ PCWSTR serverName, std::function<void()> objectsReleasedCallback)
{
    using namespace Microsoft::WRL::Wrappers;

    Microsoft::WRL::Module<Microsoft::WRL::OutOfProc>::Create(objectsReleasedCallback);

    // Get module classes
    unique_hstring_array_ptr classes;
    THROW_IF_FAILED(RoGetServerActivatableClasses(HStringReference(serverName).Get(), &classes, reinterpret_cast<DWORD*>(classes.size_address())));

    // Creation callback
    PFNGETACTIVATIONFACTORY callback = [](HSTRING name, IActivationFactory** factory) -> HRESULT {
        auto& module = Microsoft::WRL::Module<Microsoft::WRL::OutOfProc>::GetModule();
        RETURN_IF_FAILED(module.GetActivationFactory(name, factory));
        return S_OK;
    };

    // Register
    wil::unique_ro_registration_cookie registrationCookie;
    PFNGETACTIVATIONFACTORY callbacks[1] = { callback };
    THROW_IF_FAILED(RoRegisterActivationFactories(classes.get(), callbacks, static_cast<UINT32>(classes.size()), &registrationCookie));
    return registrationCookie;
}

static std::wstring ParseServerNameArgument(std::wstring_view wargv)
{
    constexpr wchar_t serverNamePrefix[] = L"-ServerName:";
    if (_wcsnicmp(wargv.data(), serverNamePrefix, wcslen(serverNamePrefix)) != 0)
    {
        THROW_HR(E_UNEXPECTED);
    }
    return { wargv.data() + wcslen(serverNamePrefix) };
}

void debugsleep()
{
    while (!IsDebuggerPresent())
    {
        Sleep(100);
    };
    DebugBreak();
}

int __stdcall wWinMain(HINSTANCE, HINSTANCE, LPWSTR wargv, int wargc) try
{
    if (wargc < 1)
    {
        THROW_HR(E_INVALIDARG);
    }


    // Parse the servername from the cmdline argument, e.g. "-ServerName:DevHome.QuietBackgroundProcesses.ElevatedServer"
    auto serverName = ParseServerNameArgument(wargv);

    bool isElevatedServer{};
    if (wil::compare_string_ordinal(serverName, L"DevHome.QuietBackgroundProcesses.Server", true) == 0)
    {
    }
    else if (wil::compare_string_ordinal(serverName, L"DevHome.QuietBackgroundProcesses.ElevatedServer", true) == 0)
    {
        isElevatedServer = true;
    }
    else
    {
        THROW_HR(E_INVALIDARG);
    }

    if (isElevatedServer)
    {
        if (!IsTokenElevated(GetCurrentProcessToken()))
        {
            SelfElevate(wargv);
            Sleep(30000);
            return 0;
        }
        else
        {
        }
    }

    auto unique_rouninitialize_call = wil::RoInitialize();

    // Enable fast rundown of COM stubs in this process to ensure that RPCSS bookkeeping is updated synchronously.
    wil::com_ptr<IGlobalOptions> pGlobalOptions;
    THROW_IF_FAILED(CoCreateInstance(CLSID_GlobalOptions, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pGlobalOptions)));
    THROW_IF_FAILED(pGlobalOptions->Set(COMGLB_RO_SETTINGS, COMGLB_FAST_RUNDOWN));
    THROW_IF_FAILED(pGlobalOptions->Set(COMGLB_EXCEPTION_HANDLING, COMGLB_EXCEPTION_DONOT_HANDLE_ANY));

    // To be safe, force quiet mode off to begin the proceedings in case we leaked the machine state previously
    if (isElevatedServer)
    {
        QuietState::TurnOff();
    }

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
    if (isElevatedServer)
    {
        QuietState::TurnOff();
    }

    return 0;
}
CATCH_RETURN()
