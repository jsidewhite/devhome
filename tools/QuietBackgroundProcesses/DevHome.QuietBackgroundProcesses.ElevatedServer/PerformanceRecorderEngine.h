// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

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

#include <Windows.Foundation.h>
#include <Windows.Foundation.Collections.h>

#include "DevHome.QuietBackgroundProcesses.h"
#include "procmon.h"

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
        STDMETHODIMP RuntimeClassInitialize() noexcept;

        // IPerformanceRecorderEngine
        STDMETHODIMP Start(int periodInMs) noexcept override;

        STDMETHODIMP Stop(ABI::DevHome::QuietBackgroundProcesses::IProcessPerformanceTable** result) noexcept override;
    private:
        unique_process_utilization_monitoring_thread m_context;
    };
}
