// DevHome.Elevation.ConsoleClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <filesystem>
#include <iostream>

#include <wil/com.h>
#include <wil/registry.h>
#include <wil/resource.h>
#include <wil/result_macros.h>
#include <wil/token_helpers.h>
#include <wil/win32_helpers.h>
#include <wil/winrt.h>

#include <windows.h>
#include <shellapi.h>

#include "DevHome.Elevation.h"

int main()
{

    // Launch elevated instance
    auto pathToZoneLaunchPad = std::filesystem::path(wil::GetModuleFileNameW().get());
    pathToZoneLaunchPad = pathToZoneLaunchPad.replace_filename(L"DevHome.Elevation.ZoneLaunchPad.exe");

    SHELLEXECUTEINFO sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    sei.lpFile = pathToZoneLaunchPad.wstring().c_str();
    sei.lpParameters = L"ZoneA";
    sei.hwnd = NULL;
    sei.nShow = SW_NORMAL;

    THROW_LAST_ERROR_IF(!ShellExecuteEx(&sei));

    // Let process finish
    wil::handle_wait(sei.hProcess);

    DWORD exitCode = 0;
    THROW_LAST_ERROR_IF(!GetExitCodeProcess(sei.hProcess, &exitCode));

    std::cout << "Hello DevHome.Elevation.ZoneLaunchPad.exe! exit code = " << std::hex << exitCode << std::endl;


    
    auto unique_rouninitialize_call = wil::RoInitialize();
    auto zoneConnectionManager = wil::GetActivationFactory<ABI::DevHome::Elevation::IZoneConnectionManagerStatics>(L"DevHome.Elevation.ZoneConnectionManager");

    zoneConnectionManager->OpenConnection(ABI::D);


    std::cout << "Hello DevHome.Elevation.ZoneLaunchPad.exe! exit code = " << std::hex << exitCode << std::endl;
}
