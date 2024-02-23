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
#include <wil/winrt.h>

#include <objbase.h>
#include <roregistrationapi.h>

#include "Timer.h"
#include "QuietState.h"
#include "Utility.h"

constexpr bool g_debugbuild =
#if _DEBUG
    true;
#else
    false;
#endif

std::mutex g_finishMutex;
std::condition_variable g_finishCondition;
bool g_lastInstanceOfTheModuleObjectIsReleased;

void waitfordebugger()
{
    if (!g_debugbuild)
    {
        return;
    }

    for (int i = 0; i < 6; i++)
    {
        if (IsDebuggerPresent())
        {
            break;
        }
        Sleep(1000);
    };
    DebugBreak();
}

static wil::unique_ro_registration_cookie RegisterWinrtClasses(_In_ PCWSTR serverName, std::function<void()> objectsReleasedCallback)
{
    using namespace Microsoft::WRL::Wrappers;

    auto& module = Microsoft::WRL::Module<Microsoft::WRL::OutOfProc>::Create(objectsReleasedCallback);

    // Get module classes
    //unique_hstring_array_ptr classes;
    //THROW_IF_FAILED(RoGetServerActivatableClasses(HStringReference(serverName).Get(), &classes, reinterpret_cast<DWORD*>(classes.size_address())));
    module.RegisterObjects();
    
    /*
    // Creation callback
    PFNGETACTIVATIONFACTORY callback = [](HSTRING name, IActivationFactory** factory) -> HRESULT {
        auto& module = Microsoft::WRL::Module<Microsoft::WRL::OutOfProc>::GetModule();
        RETURN_IF_FAILED(module.GetActivationFactory(name, factory));
        return S_OK;
    };
    */

    // Register
    //wil::unique_ro_registration_cookie registrationCookie;
    //PFNGETACTIVATIONFACTORY callbacks[1] = { callback };
    //THROW_IF_FAILED(RoRegisterActivationFactories(classes.get(), callbacks, static_cast<UINT32>(classes.size()), &registrationCookie));
    //return registrationCookie;
    return {};
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
    constexpr auto SERVER_STARTED_EVENT_NAME = L"Global\\DevHome_QuietBackgroundProcesses_ElevatedServer_Started";
    //constexpr auto SERVER_FINISHED_EVENT_NAME = L"Global\\DevHome_QuietBackgroundProcesses_ElevatedServer_Finished";

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


    //wil::unique_event elevatedServerFinishedEvent;
    if (!isElevatedServer)
    {
        ////debugsleep();
        //elevatedServerFinishedEvent.create(wil::EventOptions::ManualReset, SERVER_FINISHED_EVENT_NAME);
    }

    // Wait for the elevated server to register with COM
    if (isElevatedServer && !IsTokenElevated(GetCurrentProcessToken()))
    {
        wil::unique_event elevatedServerRunningEvent;
        elevatedServerRunningEvent.create(wil::EventOptions::ManualReset, SERVER_STARTED_EVENT_NAME);
        SelfElevate(wargv);
        elevatedServerRunningEvent.wait();
        return 0;
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
        //debugsleep();
        QuietState::TurnOff();
    }


    
    wil::com_ptr<ABI::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionManagerStatics> factory;
    if (isElevatedServer)
    {
        //factory = wil::GetActivationFactory<ABI::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionManagerStatics>(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSessionManager);
        //factory.reset(x);
    }

    // Register WRL callback when all objects are destroyed
    auto& module = Microsoft::WRL::Module<Microsoft::WRL::OutOfProc>::Create([] {
        auto msg = std::wstring(L"Main: All WRL module references released callback\n");
        OutputDebugStringW(msg.c_str());

        // The last instance object of the module is released
        {
            auto lock = std::unique_lock<std::mutex>(g_finishMutex);
            g_lastInstanceOfTheModuleObjectIsReleased = true;
        }
        g_finishCondition.notify_one();
    });

    // Register WinRT activatable classes
    module.RegisterObjects();
    auto unique_wrl_registration_cookie = wil::scope_exit([&module]() {
        module.UnregisterObjects();
    });

    // Tell the unelevated server that we've registered with COM and it may shutdown
    if (isElevatedServer)
    {
        wil::unique_event elevatedServerRunningEvent;
        elevatedServerRunningEvent.open(SERVER_STARTED_EVENT_NAME);
        elevatedServerRunningEvent.SetEvent();
    }

    // Wait for the module objects to be released and the timer threads to finish
    if (isElevatedServer)
    {
        auto lock = std::unique_lock<std::mutex>(g_finishMutex);

        // Wait for both events to complete
        g_finishCondition.wait(lock, [&isElevatedServer] {
            auto msg = std::wstring(L"Main: Wait check returns ") + std::to_wstring(g_lastInstanceOfTheModuleObjectIsReleased) + std::wstring(L"\n");
            OutputDebugStringW(msg.c_str());

            return g_lastInstanceOfTheModuleObjectIsReleased;
        });

        //elevatedServerFinishedEvent.open(SERVER_FINISHED_EVENT_NAME);
        //elevatedServerFinishedEvent.SetEvent();
    }
    else
    {
        if (!isElevatedServer)
        {
            //elevatedServerFinishedEvent.wait();
        }

        auto lock = std::unique_lock<std::mutex>(g_finishMutex);

        // Wait for both events to complete
        g_finishCondition.wait(lock, [&isElevatedServer] {
            auto msg = std::wstring(L"Main: Wait check returns ") + std::to_wstring(g_lastInstanceOfTheModuleObjectIsReleased) + std::wstring(L"\n");
            OutputDebugStringW(msg.c_str());

            return g_lastInstanceOfTheModuleObjectIsReleased;
        });
    }
    
    //LOG_IF_FAILED(factory->InvalidateSessionReference());
    factory.reset();

    // To be safe, force quiet mode off
    if (isElevatedServer)
    {
        QuietState::TurnOff();
        Timer::WaitForAllDiscardedTimersToDestruct();
    }

    return 0;
}
CATCH_RETURN()
