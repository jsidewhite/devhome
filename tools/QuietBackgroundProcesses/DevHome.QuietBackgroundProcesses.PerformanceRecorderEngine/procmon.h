// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

struct ProcessPerformanceSummary
{
    // Process info
    ULONG pid{};
    //std::wstring processName;
    //wil::unique_cotaskmem_string processName;
    wchar_t processName[14];

    // Sampling
    uint64_t sampleCount{};
    float percentCumulative{};
    float sigmaCumulative{};
};

extern "C" __declspec(dllexport) HRESULT StartMonitoringProcessUtilization(uint32_t periodInMs, void** context) noexcept;
extern "C" __declspec(dllexport) HRESULT StopMonitoringProcessUtilization(void* context) noexcept;
extern "C" __declspec(dllexport) HRESULT GetMonitoringProcessUtilization(void* context, ProcessPerformanceSummary** ppSummaries, uint32_t* summaryCount) noexcept;
extern "C" __declspec(dllexport) HRESULT DeleteMonitoringProcessUtilization(void* context) noexcept;

using unique_process_utilization_monitoring_thread = wil::unique_any<void*, decltype(&::DeleteMonitoringProcessUtilization), ::DeleteMonitoringProcessUtilization>;
