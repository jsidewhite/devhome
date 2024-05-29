// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

#include <filesystem>
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

#include "DevHome.Elevation.h"
#include "Utility.h"

ULONG_PTR GetParentProcessId()
{
    ULONG_PTR pbi[6];
    ULONG ulSize = 0;
    LONG(WINAPI * NtQueryInformationProcess)
    (HANDLE ProcessHandle, ULONG ProcessInformationClass, PVOID ProcessInformation, ULONG ProcessInformationLength, PULONG ReturnLength);
    *(FARPROC*)&NtQueryInformationProcess =
        GetProcAddress(LoadLibraryA("NTDLL.DLL"), "NtQueryInformationProcess");
    if (NtQueryInformationProcess)
    {
        if (NtQueryInformationProcess(GetCurrentProcess(), 0, &pbi, sizeof(pbi), &ulSize) >= 0 && ulSize == sizeof(pbi))
            return pbi[5];
    }
    return (ULONG_PTR)-1;
}

int __stdcall wWinMain(HINSTANCE, HINSTANCE, LPWSTR wargv, int wargc) try
{
    if (wargc < 1)
    {
        THROW_HR(E_INVALIDARG);
    }

    // Parse the target Zone to be launched
    //auto zoneName = wargv[1];
    auto zoneName = wargv;

    if (wil::compare_string_ordinal(zoneName, L"ZoneA", true) != 0)
    {
        THROW_HR(E_INVALIDARG);
    }

    // Make sure we're already elevated instance
    if (!IsTokenElevated(GetCurrentProcessToken()))
    {
        THROW_HR(E_INVALIDARG);
    }

    WaitForDebuggerIfPresent();

    auto unique_rouninitialize_call = wil::RoInitialize();

    //zoneConnectionManager->LaunchZone(zoneName);
    //wil::CoCreateInstance<ABI::DevHome::Elevation::ZoneConnectionManager>(CLSID_ZoneA, CLSCTX_LOCAL_SERVER);
    auto zoneConnectionManager = wil::GetActivationFactory<ABI::DevHome::Elevation::IZoneConnectionManagerStatics>(L"DevHome.Elevation.ZoneConnectionManager");

    // Get PID of parent process
    auto parentProcessId = GetParentProcessId();

    // Get parent process create time
    auto process = wil::unique_process_handle{ OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, parentProcessId) };
    THROW_IF_NULL_ALLOC(process.get());

    FILETIME createTime{};
    FILETIME exitTime{};
    FILETIME kernelTime{};
    FILETIME userTime{};
    THROW_IF_WIN32_BOOL_FALSE(GetProcessTimes(process.get(), &createTime, &exitTime, &kernelTime, &userTime));

    INT64 createTime64 = createTime.dwLowDateTime + ((UINT64)createTime.dwHighDateTime << 32);
    auto createTimeDatetime = ABI::Windows::Foundation::DateTime{ createTime64 };

    THROW_IF_FAILED(zoneConnectionManager->PrepareConnection(parentProcessId, createTimeDatetime, ABI::DevHome::Elevation::Zone_A, nullptr));


    return 0;
}
CATCH_RETURN()
