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

#include "QuietBackgroundProcessesSessionManager.h"
#include "QuietBackgroundProcessesSession.h"
#include "QuietState.h"
#include "Utility.h"

std::mutex g_finishMutex;
std::condition_variable g_finishCondition;
bool g_lastInstanceOfTheModuleObjectIsReleased;

template <typename FactoryT>
static HRESULT make_factory(IActivationFactory** out) noexcept
try
{
    auto factoryObject = winrt::make<FactoryT>();
    auto factoryInterface = factoryObject.as<winrt::Windows::Foundation::IActivationFactory>();
    *out = static_cast<IActivationFactory*>(winrt::detach_abi(factoryInterface));
    return S_OK;
}
CATCH_RETURN()

static wil::unique_ro_registration_cookie RegisterWinrtClasses(_In_ PCWSTR serverName, std::function<void()> objectsReleasedCallback)
{
    using namespace Microsoft::WRL::Wrappers;

    Microsoft::WRL::Module<Microsoft::WRL::OutOfProc>::Create(objectsReleasedCallback);

    // Get module classes
    unique_hstring_array_ptr classes;
    THROW_IF_FAILED(RoGetServerActivatableClasses(HStringReference(serverName).Get(), &classes, reinterpret_cast<DWORD*>(classes.size_address())));

    // Creation callback
    PFNGETACTIVATIONFACTORY callback = [](HSTRING name, IActivationFactory** factory) -> HRESULT {
        if (wil::compare_string_ordinal(WindowsGetStringRawBuffer(name, nullptr), L"DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager", true) == 0)
        {
            RETURN_IF_FAILED(make_factory<winrt::DevHome::QuietBackgroundProcesses::factory_implementation::QuietBackgroundProcessesSessionManager>(factory));
            return S_OK;
        }
        else if (wil::compare_string_ordinal(WindowsGetStringRawBuffer(name, nullptr), L"DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession", true) == 0)
        {
            RETURN_IF_FAILED(make_factory<winrt::DevHome::QuietBackgroundProcesses::factory_implementation::QuietBackgroundProcessesSession>(factory));
            return S_OK;
        }

        RETURN_HR(E_UNEXPECTED);
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

int __stdcall wWinMain(HINSTANCE, HINSTANCE, LPWSTR wargv, int wargc) try
{

    if (wargc < 1)
    {
        THROW_HR(E_INVALIDARG);
    }

    // Parse the servername from the cmdline argument, e.g. "-ServerName:DevHome.QuietBackgroundProcesses.ElevatedServer"
    auto serverName = ParseServerNameArgument(wargv);

    if (wil::compare_string_ordinal(serverName, L"DevHome.QuietBackgroundProcesses.Server", true) == 0)
    {
    }
    else if (wil::compare_string_ordinal(serverName, L"DevHome.QuietBackgroundProcesses.ElevatedServer", true) == 0)
    {
        if (!IsTokenElevated(GetCurrentProcessToken()))
        {
            SelfElevate(wargv);
            Sleep(600000);
            return 0;
        }
        else
        {
            while (!IsDebuggerPresent())
            {
                Sleep(100);
            };
            DebugBreak();
        }
    }
    else
    {
        THROW_HR(E_INVALIDARG);
    }

    
    auto unique_rouninitialize_call = wil::RoInitialize();

wil::com_ptr<IGlobalOptions> pGlobalOptions;
    THROW_IF_FAILED(CoCreateInstance(CLSID_GlobalOptions, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pGlobalOptions)));
//THROW_IF_FAILED(pGlobalOptions->Set(COMGLB_UNMARSHALING_POLICY, COMGLB_UNMARSHALING_POLICY_STRONG)); // to opt-out pass COMGLB_UNMARSHALING_POLICY_NORMAL

    // Enable fast rundown of COM stubs in this process to ensure that RPCSS bookkeeping is updated
    // synchronously during the CCI from the manager process to reflect the fact that the manager has
    // a broker interface proxy.
    // COMGLB_ENABLE_AGILE_OOP_PROXIES – makes all COM proxies agile, avoid the need to marshal and use things like AgileRef. Simpler/faster code.
    // COMGLB_ENABLE_SHARED_FTM. – avoids an allocation per agile object. By default FtmBase uses the free threaded marshaler and without this option a new unique one is allocated per object.
//THROW_IF_FAILED(pGlobalOptions->Set(COMGLB_RO_SETTINGS, COMGLB_FAST_RUNDOWN | COMGLB_ENABLE_AGILE_OOP_PROXIES | COMGLB_ENABLE_SHARED_FTM));
    
    //THROW_IF_FAILED(pGlobalOptions->Set());
    //THROW_IF_FAILED(pGlobalOptions->Set());
    THROW_IF_FAILED(pGlobalOptions->Set(COMGLB_RO_SETTINGS, COMGLB_FAST_RUNDOWN));
    THROW_IF_FAILED(pGlobalOptions->Set(COMGLB_EXCEPTION_HANDLING, COMGLB_EXCEPTION_DONOT_HANDLE_ANY));



    // To be safe, force quiet mode off to begin the proceedings in case we leaked the machine state previously
    QuietState::TurnOff();


    // Register WinRT activatable classes
    auto registrationCookie = RegisterWinrtClasses(serverName.c_str(), [] {
        __debugbreak();
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
CATCH_RETURN()
