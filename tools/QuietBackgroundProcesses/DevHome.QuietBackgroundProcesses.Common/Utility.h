// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <string>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/implements.h>
#include <wrl/module.h>
#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>
#include <wil/token_helpers.h>
#include <wil/win32_helpers.h>
#include <wil/winrt.h>

#include <windows.h>
#include <shellapi.h>

constexpr bool DEBUG_BUILD =
#if _DEBUG
    true;
#else
    false;
#endif

inline void WaitForDebuggerIfPresent()
{
    if (!DEBUG_BUILD)
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

using unique_hstring_array_ptr = wil::unique_any_array_ptr<HSTRING, wil::cotaskmem_deleter, wil::function_deleter<decltype(::WindowsDeleteString), ::WindowsDeleteString>>;

template <typename T>
struct wrl_module_object_ref
{
    wrl_module_object_ref()
    {
        auto& module = T::GetModule();
        auto count = module.IncrementObjectCount();
        auto msg = std::wstring(L"WRL: IncrementObjectCount = ") + std::to_wstring(count) + std::wstring(L"\n");
        OutputDebugStringW(msg.c_str());
    }

    ~wrl_module_object_ref()
    {
        reset();
    }

    wrl_module_object_ref(wrl_module_object_ref&& other) noexcept = default;
    wrl_module_object_ref& operator=(wrl_module_object_ref&& other) noexcept = default;

    wrl_module_object_ref(const wrl_module_object_ref&) = delete;
    wrl_module_object_ref& operator=(const wrl_module_object_ref&) = delete;

    void reset()
    {
        auto& module = T::GetModule();
        auto count = module.DecrementObjectCount();
        auto msg = std::wstring(L"WRL: DecrementObjectCount = ") + std::to_wstring(count) + std::wstring(L"\n");
        OutputDebugStringW(msg.c_str());
    }
};

using wrl_server_process_ref = wrl_module_object_ref<Microsoft::WRL::Module<Microsoft::WRL::OutOfProc>>;


inline bool IsTokenElevated(HANDLE token)
{
    auto mandatoryLabel = wil::get_token_information<TOKEN_MANDATORY_LABEL>(token);
    LONG levelRid = static_cast<SID*>(mandatoryLabel->Label.Sid)->SubAuthority[0];
    return levelRid == SECURITY_MANDATORY_HIGH_RID;
}

inline void SelfElevate(std::optional<std::wstring> const& arguments)
{
    auto path = wil::GetModuleFileNameW();

    SHELLEXECUTEINFO sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    sei.lpFile = path.get();
    sei.lpParameters = arguments.value().c_str();
    sei.hwnd = NULL;
    sei.nShow = SW_NORMAL;

    THROW_LAST_ERROR_IF(!ShellExecuteEx(&sei));
}

inline std::wstring ParseServerNameArgument(std::wstring_view wargv)
{
    constexpr wchar_t serverNamePrefix[] = L"-ServerName:";
    if (_wcsnicmp(wargv.data(), serverNamePrefix, wcslen(serverNamePrefix)) != 0)
    {
        THROW_HR(E_UNEXPECTED);
    }
    return { wargv.data() + wcslen(serverNamePrefix) };
}

inline void SetComFastRundownAndNoEhHandle()
{
    // Enable fast rundown of COM stubs in this process to ensure that RPCSS bookkeeping is updated synchronously.
    wil::com_ptr<IGlobalOptions> pGlobalOptions;
    THROW_IF_FAILED(CoCreateInstance(CLSID_GlobalOptions, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pGlobalOptions)));
    THROW_IF_FAILED(pGlobalOptions->Set(COMGLB_RO_SETTINGS, COMGLB_FAST_RUNDOWN));
    THROW_IF_FAILED(pGlobalOptions->Set(COMGLB_EXCEPTION_HANDLING, COMGLB_EXCEPTION_DONOT_HANDLE_ANY));
}
