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

int __stdcall wWinMain(HINSTANCE, HINSTANCE, LPWSTR wargv, int wargc) try
{
    WaitForDebuggerIfPresent();

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

    std::mutex mutex;
    bool comFinished{};
    std::condition_variable finishCondition;

#pragma warning(push)
#pragma warning(disable: 4324) // Avoid WRL alignment warning

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

    // Wait for all server references to release
    auto lock = std::unique_lock<std::mutex>(mutex);

    finishCondition.wait(lock, [&] {
        return comFinished;
    });

    return 0;
}
CATCH_RETURN()
