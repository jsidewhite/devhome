// Copyright (c) Microsoft Corporation and Contributors
// Licensed under the MIT license.

#include <windows.h>
#include <shellapi.h>

using unique_hstring_array_ptr = wil::unique_any_array_ptr<HSTRING, wil::cotaskmem_deleter, wil::function_deleter<decltype(::WindowsDeleteString), ::WindowsDeleteString>>;

inline bool IsTokenElevated(HANDLE token)
{
    auto mandatoryLabel = wil::get_token_information<TOKEN_MANDATORY_LABEL>(token);
    LONG levelRid = static_cast<SID*>(mandatoryLabel->Label.Sid)->SubAuthority[0];
    return levelRid == SECURITY_MANDATORY_HIGH_RID;
}

inline void SelfElevate(std::optional<std::wstring> const& arguments)
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
