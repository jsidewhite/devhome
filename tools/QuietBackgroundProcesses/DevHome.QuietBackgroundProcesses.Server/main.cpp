// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

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

#include "Utility.h"

std::mutex g_finishMutex;
std::condition_variable g_finishCondition;
bool g_lastInstanceOfTheModuleObjectIsReleased;

int __stdcall wWinMain(HINSTANCE, HINSTANCE, LPWSTR wargv, int wargc) try
{
    waitfordebugger();

    if (wargc < 1)
    {
        THROW_HR(E_INVALIDARG);
    }

    // Parse the servername from the cmdline argument, e.g. "-ServerName:DevHome.QuietBackgroundProcesses.Server"
    auto serverName = ParseServerNameArgument(wargv);

    if (wil::compare_string_ordinal(serverName, L"DevHome.QuietBackgroundProcesses.Server", true) != 0)
    {
        THROW_HR(E_INVALIDARG);
    }

    auto unique_rouninitialize_call = wil::RoInitialize();

    // Enable fast rundown of COM stubs in this process to ensure that RPCSS bookkeeping is updated synchronously.
    SetComFastRundownAndNoEhHandle();

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

    // Wait for all server references to release
    auto lock = std::unique_lock<std::mutex>(g_finishMutex);

    g_finishCondition.wait(lock, [] {
        auto msg = std::wstring(L"Main: Wait check returns ") + std::to_wstring(g_lastInstanceOfTheModuleObjectIsReleased) + std::wstring(L"\n");
        OutputDebugStringW(msg.c_str());
        return g_lastInstanceOfTheModuleObjectIsReleased;
    });

    return 0;
}
CATCH_RETURN()
