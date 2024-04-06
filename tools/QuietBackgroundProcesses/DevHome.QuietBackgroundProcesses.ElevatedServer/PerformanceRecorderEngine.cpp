// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

#include <chrono>
#include <memory>
#include <mutex>

#include <wrl/client.h>
#include <wrl/implements.h>
#include <wrl/module.h>

#include <wil/resource.h>
#include <wil/result_macros.h>
#include <wil/win32_helpers.h>
#include <wil/winrt.h>

#include <Windows.Foundation.h>
#include <Windows.Foundation.Collections.h>

#include "TimedQuietSession.h"
#include "DevHome.QuietBackgroundProcesses.h"
#include "procmon.h"


namespace ABI::DevHome::QuietBackgroundProcesses
{
    class ProcessRow :
        public Microsoft::WRL::RuntimeClass<
            Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRt>,
            IProcessRow,
            Microsoft::WRL::FtmBase>
    {
        InspectableClass(RuntimeClass_DevHome_QuietBackgroundProcesses_ProcessRow, BaseTrust);

    public:
        STDMETHODIMP RuntimeClassInitialize(ProcessPerformanceSummary summary) noexcept
        {
            m_summary = summary;
            return S_OK;
        }

        STDMETHODIMP get_Pid(__int64* value) noexcept override
        try
        {
            *value = m_summary.pid;
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_Name(HSTRING* value) noexcept override
        try
        {
            Microsoft::WRL::Wrappers::HString str;
            str.Set(m_summary.processName);
            *value = str.Detach();
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_PackageFullName(HSTRING* value) noexcept override
        try
        {
            Microsoft::WRL::Wrappers::HString str;
            str.Set(m_summary.packageFullName);
            *value = str.Detach();
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_Aumid(HSTRING* value) noexcept override
        try
        {
            Microsoft::WRL::Wrappers::HString str;
            str.Set(m_summary.aumid);
            *value = str.Detach();
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_Type(ABI::DevHome::QuietBackgroundProcesses::ProcessType* value) noexcept override
        try
        {
            *value = ProcessType_User;
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_SampleCount(unsigned __int64* value) noexcept override
        try
        {
            *value = m_summary.sampleCount;
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_PercentCumulative(float* value) noexcept override
        try
        {
            *value = m_summary.percentCumulative;
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_VarianceCumulative(float* value) noexcept override
        try
        {
            *value = m_summary.varianceCumulative;
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_Sigma4Cumulative(float* value) noexcept override
        try
        {
            *value = m_summary.sigma4Cumulative;
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_MaxPercent(float* value) noexcept override
        try
        {
            *value = m_summary.maxPercent;
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_SamplesAboveThreshold(unsigned __int32* value) noexcept override
        try
        {
            *value = m_summary.samplesAboveThreshold;
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_TotalCpuTimeInMicroseconds(unsigned __int64* value) noexcept override
        try
        {
            *value = m_summary.totalCpuTimeInMicroseconds;
            return S_OK;
        }
        CATCH_RETURN()

    private:
        ProcessPerformanceSummary m_summary;
    };

}

namespace ABI::DevHome::QuietBackgroundProcesses
{
    class ProcessPerformanceTable :
        public Microsoft::WRL::RuntimeClass<
            Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRt>,
            IProcessPerformanceTable,
            Microsoft::WRL::FtmBase>
    {
        InspectableClass(RuntimeClass_DevHome_QuietBackgroundProcesses_ProcessPerformanceTable, BaseTrust);

    public:
        STDMETHODIMP RuntimeClassInitialize(unique_process_utilization_monitoring_thread context) noexcept
        {
            m_context = std::move(context);
            return S_OK;
        }

        STDMETHODIMP get_Rows(unsigned int* valueLength, ABI::DevHome::QuietBackgroundProcesses::IProcessRow*** value) noexcept override
        try
        {
            if (m_context)
            {
                m_rows.clear();

                size_t summaryCount;
                ProcessPerformanceSummary* pSummaries;
                THROW_IF_FAILED(GetMonitoringProcessUtilization(m_context.get(), &pSummaries, &summaryCount));

                // add rows
                {
                    //wil::com_ptr<ProcessRow> obj;
                    //THROW_IF_FAILED(Microsoft::WRL::MakeAndInitialize<ProcessRow>(&obj, L"sdsdfs", ProcessType_User, 1.0));
                    //rows.push_back(std::move(obj));
                }

                for (uint32_t i = 0; i < summaryCount; i++)
                {
                    auto& summary = pSummaries[i];
                    //std::wcout << L"i=" << i << L" pid=" << summary.pid << L" name=" << str << std::endl;
                    //std::wcout << L"i="<< str << std::endl;
                    //std::wcout << L"i=" << i << L" str=" << std::endl;

                    wil::com_ptr<ProcessRow> obj;
                    //auto y = (1 + summary.percentCumulative) + 
                    THROW_IF_FAILED(Microsoft::WRL::MakeAndInitialize<ProcessRow>(&obj, summary));
                    m_rows.push_back(std::move(obj));
                }

                // m_context.reset();
            }

            //todo smart pointers
            auto list = wil::unique_cotaskmem_array_ptr<IProcessRow*>{ static_cast<IProcessRow**>(CoTaskMemAlloc(m_rows.size() * sizeof(IProcessRow*))), m_rows.size() };
            for (int i = 0; i < m_rows.size(); i++)
            {
                list[i] = m_rows[i].detach();
            }
            *valueLength = static_cast<unsigned int>(m_rows.size());
            *value = list.release();
            return S_OK;
        }
        CATCH_RETURN()

    private:
        unique_process_utilization_monitoring_thread m_context;
        std::vector<wil::com_ptr<ProcessRow>> m_rows;
    };
}

namespace ABI::DevHome::QuietBackgroundProcesses
{
    class PerformanceRecorderEngine :
        public Microsoft::WRL::RuntimeClass<
            Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRt>,
            IPerformanceRecorderEngine,
            Microsoft::WRL::FtmBase>
    {
        InspectableClass(RuntimeClass_DevHome_QuietBackgroundProcesses_PerformanceRecorderEngine, BaseTrust);

    public:
        STDMETHODIMP RuntimeClassInitialize() noexcept
        {
            return S_OK;
        }

        // IPerformanceRecorderEngine
        STDMETHODIMP Start(int periodInMs) noexcept override
        try
        {
            THROW_IF_FAILED(StartMonitoringProcessUtilization(periodInMs, &m_context));
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP Stop(ABI::DevHome::QuietBackgroundProcesses::IProcessPerformanceTable** result) noexcept override
        try
        {
            THROW_IF_FAILED(StopMonitoringProcessUtilization(m_context.get()));

            if (result)
            {
                wil::com_ptr<ProcessPerformanceTable> performanceTable;
                THROW_IF_FAILED(Microsoft::WRL::MakeAndInitialize<ProcessPerformanceTable>(&performanceTable, std::move(m_context)));
                *result = performanceTable.detach();
            }

            return S_OK;
        }
        CATCH_RETURN()

    private:
        unique_process_utilization_monitoring_thread m_context;
    };

    ActivatableClass(PerformanceRecorderEngine);
}
