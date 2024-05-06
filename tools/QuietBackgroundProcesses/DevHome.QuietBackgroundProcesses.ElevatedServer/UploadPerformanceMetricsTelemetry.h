// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <wil/resource.h>
#include <wil/result_macros.h>
#include <wil/win32_helpers.h>
#include <wil/winrt.h>

#include "DevHomeTelemetryProvider.h"
#include "DevHome.QuietBackgroundProcesses.h"

inline void UploadPerformanceMetricsTelemetry(ABI::DevHome::QuietBackgroundProcesses::IProcessPerformanceTable* performanceTable)
{
    using namespace std::chrono_literals;

    auto metricsActivity = DevHomeTelemetryProvider::QuietBackgroundProcesses_ProcessMetrics::Start(L"sdf");

    // Iterate over table
    unsigned int valueLength;
    ABI::DevHome::QuietBackgroundProcesses::IProcessRow** value;
    THROW_IF_FAILED(performanceTable->get_Rows(&valueLength, &value));

    // Sample period
    unsigned long long samplePeriod = 1s;

    //std::span<ABI::DevHome::QuietBackgroundProcesses::IProcessRow**> span{ valueLength, value };
    //for (auto& row : span)
    for (unsigned int i = 0; i < valueLength; i++)
    {
        ABI::DevHome::QuietBackgroundProcesses::IProcessRow* row = value[i];
        //if (result)
        {
            HSTRING name;
            THROW_IF_FAILED(row->get_Name(&name));

            PCWSTR nameStr = WindowsGetStringRawBuffer(name, nullptr);

            // Get max percent
            double percent;
            THROW_IF_FAILED(row->get_MaxPercent(&percent));

            // Upload rows with max percent > 20
            if (percent > 20)
            {
                metricsActivity.ProcessInfo(nameStr, percent);
            }

            // Get sample count
            unsigned long long sampleCount;
            THROW_IF_FAILED(row->get_SampleCount(&sampleCount));

            // Upload rows with sigma4 > 4
            double sigma4Cumulative;
            THROW_IF_FAILED(row->get_Sigma4Cumulative(&sigma4Cumulative));

            double sigma4 = sigma4Cumulative / sampleCount * samplePeriod;
        }
    }
}
