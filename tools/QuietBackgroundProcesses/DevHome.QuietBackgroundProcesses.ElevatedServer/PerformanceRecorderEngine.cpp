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

#include "TimedQuietSession.h"

#include "DevHome.QuietBackgroundProcesses.h"

extern "C" __declspec(dllexport) double GetProcessCpuUsage(DWORD processId);

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
        STDMETHODIMP RuntimeClassInitialize() noexcept
        {
            return S_OK;
        }

        STDMETHODIMP get_Rows(__FIVector_1_DevHome__CQuietBackgroundProcesses__CProcessRow** value) noexcept override
        try
        {
            *value = nullptr;
            return S_OK;
        }
        CATCH_RETURN()
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
        STDMETHODIMP Start(__int64* result) noexcept override
        try
        {
            *result = 0;
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP Stop() noexcept override
        try
        {
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP GetProcessCpuUsage2(unsigned int processId, unsigned __int64* value) noexcept override
        try
        {
            auto x = ::GetProcessCpuUsage(processId);
            *value = *reinterpret_cast<uint64_t*>(&x);
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP GetProcessPerformanceTable(ABI::DevHome::QuietBackgroundProcesses::IProcessPerformanceTable** result) noexcept override
        try
        {
            wil::com_ptr<ProcessPerformanceTable> obj;
            THROW_IF_FAILED(Microsoft::WRL::MakeAndInitialize<ProcessPerformanceTable>(&obj));
            *result = obj.detach();
            return S_OK;
        }
        CATCH_RETURN()
    };

    ActivatableClass(PerformanceRecorderEngine);
}
