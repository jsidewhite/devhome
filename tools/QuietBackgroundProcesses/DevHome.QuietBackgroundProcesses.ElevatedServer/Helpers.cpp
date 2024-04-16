// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

#include <fstream>
#include <vector>
#include <span>
#include <string>

#include <wrl/client.h>
#include <wrl/implements.h>
#include <wil/com.h>
#include <wil/resource.h>

#include "DevHome.QuietBackgroundProcesses.h"
#include "PerformanceRecorderEngine.h"
#include "Helpers.h"

// Write vector to disk
void writeVectorToFile(const std::span<ProcessPerformanceSummary>& data, const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
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

// Read vector from disk
void readVectorFromFile(std::vector<ProcessPerformanceSummary>& data, const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open())
    {
        // Handle error
        return;
    }

    ProcessPerformanceSummary item;
    while (file.read(reinterpret_cast<char*>(&item), sizeof(ProcessPerformanceSummary)))
    {
        data.push_back(item);
    }

    file.close();
}

/*
// Write the performance .csv data to disk
HRESULT WritePerformanceCsvDataToDisk(ABI::DevHome::QuietBackgroundProcesses::IPerformanceRecorderEngine* engine)
try
{
    using namespace ABI::DevHome::QuietBackgroundProcesses;

    wil::unique_cotaskmem_array_ptr<ProcessPerformanceSummary> summaries;
    THROW_IF_FAILED(GetMonitoringProcessUtilization(engine, summaries.addressof(), summaries.size_address()));

    // Add rows
    std::span<ProcessPerformanceSummary> data(summaries.get(), summaries.size());
    writeVectorToFile(data, "c:\\t\\performance.csv");

    return S_OK;
}
CATCH_RETURN()
*/

// Write the performance .csv data to disk
HRESULT WritePerformanceCsvDataToDisk(const std::span<ProcessPerformanceSummary>& data)
try
{
    using namespace ABI::DevHome::QuietBackgroundProcesses;

    // Add rows
    //std::span<ProcessPerformanceSummary> data(summaries.get(), summaries.size());
    writeVectorToFile(data, "c:\\t\\performance.bin");

    return S_OK;
}
CATCH_RETURN()

// Read the performance .csv data from disk
HRESULT ReadPerformanceCsvDataFromDisk(std::vector<ProcessPerformanceSummary>& data)
try
{
    using namespace ABI::DevHome::QuietBackgroundProcesses;

    // Add rows
    readVectorFromFile(data, "c:\\t\\performance.bin");

    return S_OK;
}
CATCH_RETURN()
