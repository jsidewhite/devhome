// DevHome.Elevation.ConsoleClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <filesystem>
#include <iostream>

#include <wrl/client.h>
#include <wrl/implements.h>
#include <wrl/module.h>

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

int main() try
{
    std::cout << "Create path..." << std::endl;

    // Launch elevated instance
    auto pathToZoneLaunchPad = std::filesystem::path(wil::GetModuleFileNameW().get());
    pathToZoneLaunchPad = pathToZoneLaunchPad.replace_filename(L"DevHome.Elevation.ZoneLaunchPad.exe");

    auto pathString = pathToZoneLaunchPad.wstring();

    SHELLEXECUTEINFO sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    //sei.lpFile = LR"(W:\repo\elevation\src\bin\x64\Debug\net8.0-windows10.0.22621.0\AppX\DevHome.Elevation.ZoneLaunchPad.exe)";
    sei.lpFile = pathString.c_str();
    sei.lpParameters = L"ZoneA";
    sei.hwnd = NULL;
    sei.nShow = SW_SHOWNORMAL;
    //sei.nShow = SW_HIDE;

    std::cout << "ShellExecute..." << std::endl;

    THROW_LAST_ERROR_IF(!ShellExecuteEx(&sei));

    std::cout << "Wait..." << std::endl;

    // Let process finish
    wil::handle_wait(sei.hProcess);

    std::cout << "GetExitCodeProcess..." << std::endl;

    DWORD exitCode = 0;
    THROW_LAST_ERROR_IF(!GetExitCodeProcess(sei.hProcess, &exitCode));

    std::cout << "Hello DevHome.Elevation.ZoneLaunchPad.exe! exit code = " << std::hex << exitCode << std::endl;


    
    auto unique_rouninitialize_call = wil::RoInitialize();
    auto zoneConnectionManager = wil::GetActivationFactory<ABI::DevHome::Elevation::IZoneConnectionManagerStatics>(L"DevHome.Elevation.ZoneConnectionManager");

    wil::com_ptr<ABI::DevHome::Elevation::IZoneConnection> zoneConnection;
    THROW_IF_FAILED(zoneConnectionManager->OpenConnection(Microsoft::WRL::Wrappers::HStringReference(L"abc").Get(), &zoneConnection));

    auto zoneA = zoneConnection.query<ABI::DevHome::Elevation::IZoneA>();

    unsigned int zoneAName;
    THROW_IF_FAILED(zoneA->GetName(&zoneAName));


    std::cout << "zoneAName = " << zoneAName << std::endl;


    Sleep(10000);

    return 0;
}
catch (...)
{
    std::cout << "exception = " << wil::ResultFromCaughtException() << std::endl;
    Sleep(10000);
}
