// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

#include <fstream>
#include <numeric>
#include <span>
#include <string>
#include <vector>

#include <wil/resource.h>
#include <wil/result_macros.h>
#include <wil/win32_helpers.h>

#include "DevHomeTelemetryProvider.h"
#include "PerformanceRecorderEngine.h"
#include "Helpers.h"

void WritePerformanceDataToDisk(_In_ PCWSTR path, const std::span<ProcessPerformanceSummary>& data)
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open())
    {
        // Handle error
        return;
    }

    for (const auto& item : data)
    {
        file.write(reinterpret_cast<const char*>(&item), sizeof(ProcessPerformanceSummary));
    }

    file.close();
}

std::vector<ProcessPerformanceSummary> ReadPerformanceDataFromDisk(_In_ PCWSTR path)
{
    std::vector<ProcessPerformanceSummary> data;

    std::ifstream file(path, std::ios::binary);
    THROW_WIN32_IF(ERROR_SHARING_VIOLATION, !file.is_open());

    ProcessPerformanceSummary item;
    while (file.read(reinterpret_cast<char*>(&item), sizeof(ProcessPerformanceSummary)))
    {
        data.push_back(item);
    }

    file.close();
    return data;
}

struct ComputerInformation
{
    DWORD processorCount;
    std::wstring processor;
    std::wstring motherboard;
    DWORD ram;
};

// Get computer information
ComputerInformation GetComputerInformation()
{
    ComputerInformation computerInfo;

    // Get processor information
    SYSTEM_INFO systemInfo = { 0 };
    GetSystemInfo(&systemInfo);
    computerInfo.processorCount = systemInfo.dwNumberOfProcessors;

    // Get processor make and model using win32 apis
    wil::unique_hkey hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        wchar_t processorName[256] = { 0 };
        DWORD processorNameSize = sizeof(processorName);
        if (RegQueryValueEx(hKey.get(), L"ProcessorNameString", nullptr, nullptr, reinterpret_cast<BYTE*>(processorName), &processorNameSize) == ERROR_SUCCESS)
        {
            computerInfo.processor = processorName;
        }
    }

    // Get motherboard make and model using win32 apis
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        wchar_t biosName[256] = { 0 };
        DWORD biosNameSize = sizeof(biosName);
        if (RegQueryValueEx(hKey.get(), L"BaseBoardProduct", nullptr, nullptr, reinterpret_cast<BYTE*>(biosName), &biosNameSize) == ERROR_SUCCESS)
        {
            computerInfo.motherboard = biosName;
        }
    }

    // Get ram amount using win32 apis
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\RESOURCEMAP\\System Resources\\Physical Memory", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD ramSize = 0;
        DWORD ramSizeSize = sizeof(ramSize);
        if (RegQueryValueEx(hKey.get(), L".Translated", nullptr, nullptr, reinterpret_cast<BYTE*>(&ramSize), &ramSizeSize) == ERROR_SUCCESS)
        {
            computerInfo.ram = ramSize / 1024 / 1024;
        }
    }

    return computerInfo;
}

void UploadPerformanceDataTelemetry(const std::span<ProcessPerformanceSummary>& data)
{
    using namespace std::chrono_literals;

    enum class UploadReason
    {
        None,
        MaxPercent,
        Sigma4,
        AveragePercent,
        SearchIndexer,
    };

    struct UploadItem
    {
        UploadReason reason;
        ProcessPerformanceSummary data;
    };

    auto activity = DevHomeTelemetryProvider::QuietBackgroundProcesses_PerformanceMetrics::Start(L"sdf");

    // Upload computer information
    auto computerInformation = GetComputerInformation();
    activity.ComputerInfo(
        computerInformation.processorCount,
        computerInformation.processor.c_str(),
        computerInformation.motherboard.c_str(),
        computerInformation.ram);

    std::chrono::seconds samplingPeriod = 1s;

    // Calculate item percentages by category
    double percentageTotal{};
    double totalCpuTimeInMicroseconds{};
    for (const auto& item : data)
    {
        percentageTotal += item.percentCumulative / item.sampleCount * samplingPeriod.count();
        totalCpuTimeInMicroseconds += item.totalCpuTimeInMicroseconds;
    }

    std::vector<UploadItem> itemsToUpload;
    for (const auto& item : data)
    {
        if (item.maxPercent >= 20.0)
        {
            // Add item to list to upload
            itemsToUpload.emplace_back(UploadReason::MaxPercent, item);
        }
        else if (item.sigma4Cumulative / item.sampleCount * samplingPeriod.count() >= 4.0)
        {
            // Add item to list to upload
            itemsToUpload.emplace_back(UploadReason::Sigma4, item);
        }
        else if (item.percentCumulative / item.sampleCount * samplingPeriod.count() >= 1.0)
        {
            // Add item to list to upload
            itemsToUpload.emplace_back(UploadReason::AveragePercent, item);
        }
        else if (wil::compare_string_ordinal(item.name, L"SearchIndexer.exe", true) == 0)
        {
            // Add item to list to upload
            itemsToUpload.emplace_back(UploadReason::SearchIndexer, item);
        }

    }

    // Get windows path
    wchar_t windowsPath[MAX_PATH];
    GetWindowsDirectory(windowsPath, ARRAYSIZE(windowsPath));

    // Get system32 path
    wchar_t system32Path[MAX_PATH];
    GetSystemDirectory(system32Path, ARRAYSIZE(system32Path));

    for (const auto& itemToUpload : itemsToUpload)
    {
        activity.ProcessInfo(
            itemToUpload.reason,
            wil::compare_string_ordinal(itemToUpload.data.path, system32Path, true) == 0,
            itemToUpload.data.name.c_str(),
            itemToUpload.data.category.c_str(),
            itemToUpload.data.packageFullName.c_str(),

            itemToUpload.data.sampleCount,
            itemToUpload.data.maxPercent,
            itemToUpload.data.sigma4Cumulative / itemToUpload.data.sampleCount * samplingPeriod.count(),
            itemToUpload.data.percentCumulative / itemToUpload.data.sampleCount * samplingPeriod.count(),
            itemToUpload.data.totalCpuTimeInMicroseconds,
            );
    }
}
