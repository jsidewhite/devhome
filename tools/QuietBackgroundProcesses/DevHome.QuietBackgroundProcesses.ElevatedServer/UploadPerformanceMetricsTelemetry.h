// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <wil/resource.h>
#include <wil/result_macros.h>
#include <wil/win32_helpers.h>
#include <wil/winrt.h>

#include "DevHomeTelemetryProvider.h"
#include "DevHome.QuietBackgroundProcesses.h"

// Enum for reasons to upload process row
enum class UploadReason : int
{
    None,
    MaxPercent,
    Sigma4,
    Both
};

// Upload process row
inline void UploadProcessRow(DevHomeTelemetryProvider::QuietBackgroundProcesses_ProcessMetrics& activity, UploadReason reason, ABI::DevHome::QuietBackgroundProcesses::IProcessRow* row)
{
    // Get name
    HSTRING name;
    THROW_IF_FAILED(row->get_Name(&name));
    PCWSTR nameStr = WindowsGetStringRawBuffer(name, nullptr);

    // Get sample count
    unsigned long long sampleCount;
    THROW_IF_FAILED(row->get_SampleCount(&sampleCount));

    // Get max percent
    double percent;
    THROW_IF_FAILED(row->get_MaxPercent(&percent));

    // Get sigma4
    double sigma4Cumulative;
    THROW_IF_FAILED(row->get_Sigma4Cumulative(&sigma4Cumulative));
    double sigma4 = sigma4Cumulative / sampleCount;

    // Upload row
    activity.ProcessInfo((int)reason, nameStr, percent, sigma4);
}

inline void UploadPerformanceMetricsTelemetry(ABI::DevHome::QuietBackgroundProcesses::IProcessPerformanceTable* performanceTable)
{
    using namespace std::chrono_literals;

    auto metricsActivity = DevHomeTelemetryProvider::QuietBackgroundProcesses_ProcessMetrics::Start(L"sdf");

    // Iterate over table
    unsigned int valueLength;
    ABI::DevHome::QuietBackgroundProcesses::IProcessRow** value;
    THROW_IF_FAILED(performanceTable->get_Rows(&valueLength, &value));

    // Sample period
    std::chrono::seconds samplePeriod = 1s;

    //std::span<ABI::DevHome::QuietBackgroundProcesses::IProcessRow**> span{ valueLength, value };
    //for (auto& row : span)
    for (unsigned int i = 0; i < valueLength; i++)
    {
        // Get process row
        ABI::DevHome::QuietBackgroundProcesses::IProcessRow* row = value[i];

        // Get name
        HSTRING name;
        THROW_IF_FAILED(row->get_Name(&name));
        PCWSTR nameStr = WindowsGetStringRawBuffer(name, nullptr);

        // Get sample count
        unsigned long long sampleCount;
        THROW_IF_FAILED(row->get_SampleCount(&sampleCount));

        // Upload rows with max percent > 20
        double percent;
        THROW_IF_FAILED(row->get_MaxPercent(&percent));
        if (percent > 20)
        {
            UploadProcessRow(metricsActivity, UploadReason::MaxPercent, row);
            continue;
        }

        // Upload rows with sigma4 > 4
        double sigma4Cumulative;
        THROW_IF_FAILED(row->get_Sigma4Cumulative(&sigma4Cumulative));
        double sigma4 = sigma4Cumulative / sampleCount * samplePeriod;
        if (sigma4 > 4)
        {
            UploadProcessRow(metricsActivity, UploadReason::Sigma4, row);
        }
    }
}
