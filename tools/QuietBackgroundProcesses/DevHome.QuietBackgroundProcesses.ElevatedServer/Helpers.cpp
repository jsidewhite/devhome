// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

#include <fstream>
#include <vector>
#include <span>
#include <string>

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
    {
        UploadReason reason;
        ProcessPerformanceSummary data;
    };

    auto activity = DevHomeTelemetryProvider::QuietBackgroundProcesses_ProcessMetrics::Start(L"sdf");

    std::chrono::seconds samplingPeriod = 1s;

    std::vector<UploadItem> itemsToUpload;

    for (const auto& item : data)
    {
        if (item.maxPercent > 20.0)
        {
            // Add item to list to upload
            itemsToUpload.emplace_back(UploadReason::MaxPercent, item);
        }
        else if (item.sigma4Cumulative / item.sampleCount * samplingPeriod > 4)
        {
            // Add item to list to upload
            itemsToUpload.emplace_back(UploadReason::Sigma4, item);
        }
        else if (item.percentCumulative / item.sampleCount * samplingPeriod > 1)
        {
            // Add item to list to upload
            itemsToUpload.emplace_back(UploadReason::AveragePercent, item);
        }
        else if (wil::compare_string_ordinal(item.name, L"SearchIndexer.exe") == 0)
        {
            // Add item to list to upload
            itemsToUpload.emplace_back(UploadReason::SearchIndexer, item);
        }

        // Calculate item percentages by category


    }
}
