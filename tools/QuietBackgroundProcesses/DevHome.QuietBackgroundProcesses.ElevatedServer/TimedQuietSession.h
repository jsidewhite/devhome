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

#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager.h"

#include "Timer.h"
#include "QuietState.h"

using ElevatedServerReference = wrl_server_process_ref;

struct UnelevatedServerReference
{
    wil::com_ptr<ABI::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionManagerStatics> m_reference;
    UnelevatedServerReference() :
        m_reference(wil::GetActivationFactory<ABI::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionManagerStatics>(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSessionManager))
    {
    }

    void reset()
    {
        m_reference.reset();
    }
};

struct TimedQuietSession
{
    TimedQuietSession(std::chrono::seconds seconds)
    {
        m_timer = std::make_unique<Timer>(seconds, [this]() {
            auto lock = std::scoped_lock(m_mutex);
            Deactivate();
        });

        // Turn on quiet mode
        m_quietState = QuietState::TurnOn();
    }

    TimedQuietSession(TimedQuietSession&& other) noexcept = default;
    TimedQuietSession& operator=(TimedQuietSession&& other) noexcept = default;

    TimedQuietSession(const TimedQuietSession&) = delete;
    TimedQuietSession& operator=(const TimedQuietSession&) = delete;

    int64_t TimeLeftInSeconds()
    {
        auto lock = std::scoped_lock(m_mutex);
        return m_timer->TimeLeftInSeconds();
    }

    bool IsActive()
    {
        auto lock = std::scoped_lock(m_mutex);
        return (bool)m_quietState;
    }

    void Cancel()
    {
        auto lock = std::scoped_lock(m_mutex);

        Deactivate();
        m_timer->Cancel();

        // Destruct timer on another thread because it's destructor is blocking
        auto destructionThread = std::thread([timer = std::move(m_timer)]() {
            // destruct timer here
        });

        destructionThread.detach();
    }

private:
    void Deactivate()
    {
        // Turn off quiet mode
        m_quietState.reset();

        // Release lifetime handles to this elevated server and unelevated client server
        m_referenceUnelevated.reset();
        m_referenceElevated.reset();
    }

    UnelevatedServerReference m_referenceUnelevated;
    ElevatedServerReference m_referenceElevated;

    QuietState::unique_quietwindowclose_call m_quietState{ false };
    std::unique_ptr<Timer> m_timer;
    std::mutex m_mutex;
};


