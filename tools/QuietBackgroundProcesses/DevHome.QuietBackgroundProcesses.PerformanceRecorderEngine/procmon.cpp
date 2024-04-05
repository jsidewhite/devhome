// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <map>
#include <mutex>
#include <span>
#include <string>
#include <thread>

#include <wil/token_helpers.h>
#include <wil/win32_helpers.h>

#include <windows.h>
#include <appmodel.h>
#include <psapi.h>

#include "procmon.h"

#define CPU_TIME_ABOVE_THRESHOLD_STRIKE_VALUE 0.015f

struct ProcessPerformanceInfo
{
    // Process id
    wil::unique_process_handle process;
    ULONG pid{};

    // Process info
    std::wstring processName;
    std::wstring processPath;
    std::wstring packageFullName;
    std::wstring aumid;
    FILETIME createTime{};

    // CPU times
    FILETIME startUserTime{};
    FILETIME startKernelTime{};
    FILETIME previousUserTime{};
    FILETIME currentUserTime{};
    FILETIME previousKernelTime{};
    FILETIME currentKernelTime{};

    // Sampling
    uint64_t sampleCount{};
    float percentCumulative{};
    float varianceCumulative{};
    float sigma4Cumulative{};
    float maxPercent{};
    uint32_t samplesAboveThreshold{};
};

template<typename T>
wil::unique_cotaskmem_array_ptr<T> make_unique_cotaskmem_array_ptr(size_t numOfElements)
{
    wil::unique_cotaskmem_array_ptr<T> result;
    T* ptr = reinterpret_cast<T*>(CoTaskMemAlloc(sizeof(T) * numOfElements));
    THROW_IF_NULL_ALLOC(ptr);
    *result.addressof() = ptr;
    *result.size_address() = numOfElements;
    return result;
}

std::chrono::file_clock::time_point FileTimeToTimePoint(const FILETIME& fileTime)
{
    ULARGE_INTEGER uli;
    uli.LowPart = fileTime.dwLowDateTime;
    uli.HighPart = fileTime.dwHighDateTime;
    std::chrono::file_clock::duration d{ (static_cast<int64_t>(fileTime.dwHighDateTime) << 32) | fileTime.dwLowDateTime };
    std::chrono::file_clock::time_point tp{ d };
    return tp;
}

std::string FiletimeToString(const FILETIME& ft)
{
    std::chrono::file_clock::duration d{ (static_cast<int64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime };
    std::chrono::file_clock::time_point tp{ d };
    return std::format("{:%Y-%m-%d %H:%M}\n", tp);
}

std::chrono::microseconds CpuTimeDuration(FILETIME previous, FILETIME current)
{
    if (CompareFileTime(&previous, &current) >= 0)
    {
        return std::chrono::microseconds(0);
    }

    auto filetimeDeltaIn100ns = FileTimeToTimePoint(current) - FileTimeToTimePoint(previous);
    auto durationMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(filetimeDeltaIn100ns);
    return durationMicroseconds;
}

int GetVirtualNumCpus()
{
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
}

template<std::size_t size>
std::span<DWORD> GetPids(DWORD (&pidArray)[size])
{
    DWORD needed;
    THROW_IF_WIN32_BOOL_FALSE(EnumProcesses(pidArray, sizeof(pidArray), &needed));
    return { &pidArray[0], needed / sizeof(DWORD) };
}

std::wstring GetPackageFullNameFromTokenHelper(HANDLE token)
{
    wchar_t packageFullName[PACKAGE_FULL_NAME_MAX_LENGTH + 1]{};
    uint32_t packageFullNameLength = ARRAYSIZE(packageFullName);
    THROW_IF_WIN32_ERROR(GetPackageFullNameFromToken(token, &packageFullNameLength, packageFullName));
    return { packageFullName };
}

std::wstring GetAppUserModelIdFromTokenHelper(HANDLE token)
{
    wchar_t aumid[APPLICATION_USER_MODEL_ID_MAX_LENGTH]{};
    uint32_t aumidLength = ARRAYSIZE(aumid);
    THROW_IF_WIN32_ERROR(GetApplicationUserModelIdFromToken(token, &aumidLength, aumid));
    return { aumid };
}

ProcessPerformanceInfo MakeProcessPerformanceInfo(DWORD processId)
{
    auto process = wil::unique_process_handle{ OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId) };
    THROW_LAST_ERROR_IF(!process);

    wil::unique_cotaskmem_string processName;
    if (FAILED(wil::GetModuleFileNameExW(process.get(), NULL, processName)))
    {
        processName = wil::make_cotaskmem_string(L"<unknown>");
    }

    auto path = std::filesystem::path(processName.get());

    FILETIME createTime, exitTime, kernelTime, userTime;
    THROW_IF_WIN32_BOOL_FALSE(GetProcessTimes(process.get(), &createTime, &exitTime, &kernelTime, &userTime));

    wil::unique_handle processToken;
    THROW_IF_WIN32_BOOL_FALSE(OpenProcessToken(process.get(), TOKEN_QUERY, &processToken));

    auto info = ProcessPerformanceInfo{};
    info.process = std::move(process);
    info.pid = processId;
    info.processName = path.filename().wstring();
    info.processPath = path.parent_path().wstring();
    info.packageFullName = GetPackageFullNameFromTokenHelper(processToken.get());
    info.aumid = GetAppUserModelIdFromTokenHelper(processToken.get());
    info.createTime = createTime;

    // Start times
    info.startUserTime = userTime;
    info.startKernelTime = kernelTime;

    info.previousUserTime = userTime;
    info.currentUserTime = userTime;
    info.previousKernelTime = kernelTime;
    info.currentKernelTime = kernelTime;

    return info;
}

bool UpdateProcessPerformanceInfo(ProcessPerformanceInfo& info)
{
    FILETIME createTime, exitTime, kernelTime, userTime;
    THROW_IF_WIN32_BOOL_FALSE(GetProcessTimes(info.process.get(), &createTime, &exitTime, &kernelTime, &userTime));

    if (exitTime.dwHighDateTime != 0 || exitTime.dwLowDateTime != 0)
    {
        return false;
    }

    info.previousUserTime = info.currentUserTime;
    info.currentUserTime = userTime;
    info.previousKernelTime = info.currentKernelTime;
    info.currentKernelTime = kernelTime;

    return true;
}

struct cancellation_mechanism
{
    std::atomic<bool> m_cancelled{};
    std::mutex m_mutex;
    std::condition_variable m_cancelCondition;

    void cancel()
    {
        auto lock = std::scoped_lock(m_mutex);
        m_cancelled = true;
        m_cancelCondition.notify_all();
    }

    bool wait_for_cancel(std::chrono::milliseconds duration)
    {
        auto lock = std::unique_lock<std::mutex>(m_mutex);
        auto cancelHappened = m_cancelCondition.wait_for(lock, duration, [this] {
            return m_cancelled.load();
        });
        return cancelHappened;
    }
};

struct MonitorThread
{
    cancellation_mechanism m_cancellationMechanism;
    std::thread m_thread;
    std::mutex m_dataMutex;

    // Tracking all our process infos
    std::map<ULONG, ProcessPerformanceInfo> m_infos;

    MonitorThread(std::chrono::milliseconds periodMs)
    {
        if (periodMs.count() <= 0)
        {
            THROW_HR(E_INVALIDARG);
        }

        m_thread = std::thread([this, periodMs]() {
            try
            {
                auto numCpus = GetVirtualNumCpus();

                auto previousSnapshotTime = std::chrono::steady_clock::now();

                while (true)
                {
                    if (m_cancellationMechanism.m_cancelled)
                    {
                        break;
                    }

                    auto currentTime = std::chrono::steady_clock::now();

                    std::chrono::microseconds totalMicroseconds{};

                    DWORD pidArray[1024];
                    auto pids = GetPids(pidArray);

                    {
                        auto lock = std::scoped_lock(m_dataMutex);

                        for (auto& pid : pids)
                        {
                            try
                            {
                                // Ignore process "0" - the 'SYSTEM 'System' process
                                if (pid == 0)
                                {
                                    continue;
                                }

                                // Make a new entry
                                if (m_infos.find(pid) == m_infos.end())
                                {
                                    auto info = MakeProcessPerformanceInfo(pid);
                                    m_infos[pid] = std::move(info);
                                    continue;
                                }

                                // Get entry
                                auto& info = m_infos[pid];
                                if (!UpdateProcessPerformanceInfo(info))
                                {
                                    // m_infos.erase(pid);
                                }

                                // Collect cpuTime for process
                                auto cpuTime = CpuTimeDuration(info.previousUserTime, info.currentUserTime);
                                cpuTime += CpuTimeDuration(info.previousKernelTime, info.currentKernelTime);

                                float percent = (float)cpuTime.count() / std::chrono::duration_cast<std::chrono::microseconds>(periodMs).count() / (float)numCpus;
                                auto variance = (float)std::pow(percent * 100.0, 2.0f);
                                auto sigma4 = (float)std::pow(percent * 100.0, 4.0f);

#if 1
                                //todo:jw
                                if (percent > 0.01f)
                                {
                                    std::cout << "PID percent: " << pid << " = " << (100.0 * percent) << " %" << std::endl;
                                }
#endif

                                info.sampleCount++;
                                info.percentCumulative += percent;
                                info.varianceCumulative += variance;
                                info.sigma4Cumulative += sigma4;
                                if (percent > info.maxPercent)
                                {
                                    info.maxPercent = percent;
                                }
                                if (percent > CPU_TIME_ABOVE_THRESHOLD_STRIKE_VALUE)
                                {
                                    info.samplesAboveThreshold++;
                                }

                                totalMicroseconds += cpuTime;
                            }
                            CATCH_LOG();
                        }
                    }

#if 1
                    //todo:jw
                    float percent = (float)totalMicroseconds.count() / std::chrono::duration_cast<std::chrono::microseconds>(periodMs).count() / (float)numCpus;
                    std::cout << "Total percent: " << (100.0 * percent) << " %" << std::endl;
#endif

                    previousSnapshotTime = currentTime;

                    // Wait for interval period or user cancellation
                    if (m_cancellationMechanism.wait_for_cancel(periodMs))
                    {
                        // User cancelled
                        break;
                    }
                }
            }
            CATCH_LOG();
        });
    }

    void Cancel()
    {
        m_cancellationMechanism.cancel();
        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    template <size_t N>
    void copystr(wchar_t(& dst)[N], const std::wstring& src)
    {
        wcscpy_s(dst, N, src.substr(0, N - 1).c_str());
    }

    std::vector<ProcessPerformanceSummary> GetProcessPerformanceSummaries()
    {
        auto lock = std::scoped_lock(m_dataMutex);

        std::vector<ProcessPerformanceSummary> summaries;
        for (auto const& [key, info] : m_infos)
        {
            auto summary = ProcessPerformanceSummary{};
            auto totalUserTime = CpuTimeDuration(info.startUserTime, info.currentUserTime);
            auto totalKernelTime = CpuTimeDuration(info.startKernelTime, info.currentKernelTime);

            // Process info
            summary.pid = info.pid;
            copystr(summary.processName, info.processName);
            copystr(summary.packageFullName, info.packageFullName);
            copystr(summary.aumid, info.aumid);

            // Sampling
            summary.sampleCount = info.sampleCount;
            summary.percentCumulative = info.percentCumulative;
            summary.varianceCumulative = info.varianceCumulative;
            summary.sigma4Cumulative = info.sigma4Cumulative;
            summary.maxPercent = info.maxPercent;
            summary.samplesAboveThreshold = info.samplesAboveThreshold;

            // Other
            summary.totalCpuTimeInMicroseconds = totalUserTime.count() + totalKernelTime.count();

            if (summary.sampleCount <= 0)
            {
                summary.sampleCount = 0;
            }

            summaries.push_back(summary);
        }
        return summaries;
    }
};

//
// Exports
//

extern "C" __declspec(dllexport) HRESULT StartMonitoringProcessUtilization(uint32_t periodInMs, void** context) noexcept
try
{
    auto periodMs = std::chrono::milliseconds(periodInMs);
    auto monitorThread = std::make_unique<MonitorThread>(periodMs);
    *context = static_cast<void*>(monitorThread.release());
    return S_OK;
}
CATCH_RETURN()

extern "C" __declspec(dllexport) HRESULT StopMonitoringProcessUtilization(void* context) noexcept
try
{
    auto monitorThread = reinterpret_cast<MonitorThread*>(context);
    monitorThread->Cancel();
    return S_OK;
}
CATCH_RETURN()

extern "C" __declspec(dllexport) HRESULT DeleteMonitoringProcessUtilization(void* context) noexcept
try
{
    if (!context)
    {
        return S_OK;
    }
    auto monitorThread = std::unique_ptr<MonitorThread>(reinterpret_cast<MonitorThread*>(context));
    monitorThread->Cancel();
    monitorThread.reset();
    return S_OK;
}
CATCH_RETURN()

extern "C" __declspec(dllexport) HRESULT GetMonitoringProcessUtilization(void* context, ProcessPerformanceSummary** ppSummaries, size_t* summaryCount) noexcept
try
{
    auto monitorThread = reinterpret_cast<MonitorThread*>(context);
    auto summaries = monitorThread->GetProcessPerformanceSummaries();

    // Alloc summaries block
    auto ptrSummaries = make_unique_cotaskmem_array_ptr<ProcessPerformanceSummary>(summaries.size());
    auto i = 0;
    for (auto const& summary : summaries)
    {
        auto& dst = ptrSummaries.get()[i++];
        dst = summary;
    }

    *summaryCount = ptrSummaries.size();
    *ppSummaries = ptrSummaries.release();

    return S_OK;
}
CATCH_RETURN()
