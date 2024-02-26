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

struct ElevatedServerReference
{
    wrl_server_process_ref m_reference;

    void reset()
    {
        m_reference.reset();
    }
};

struct KeepAliveTimer
{
    // Cleanup functions
    static std::thread GetDiscardThread();
    static void Discard(std::unique_ptr<KeepAliveTimer> timer);

    KeepAliveTimer(std::chrono::seconds seconds) :
        m_timer(seconds, [this]() {
            Disconnect();
        })
    {
        // Turn on quiet mode
        m_quietState = QuietState::TurnOn();
    }

    KeepAliveTimer(KeepAliveTimer&& other) noexcept = default;
    KeepAliveTimer& operator=(KeepAliveTimer&& other) noexcept = default;

    KeepAliveTimer(const KeepAliveTimer&) = delete;
    KeepAliveTimer& operator=(const KeepAliveTimer&) = delete;

    int64_t TimeLeftInSeconds()
    {
        return m_timer.TimeLeftInSeconds();
    }

    bool IsActive()
    {
        auto lock = std::scoped_lock(m_mutex);
        //todo:jw lock?
        return (bool)m_quietState;
    }

    void Cancel()
    {
        Disconnect();

        //todo:jw
        m_timer.Cancel();
    }

private:
    void Disconnect()
    {
        auto lock = std::scoped_lock(m_mutex);
        // Turn off quiet mode
        m_quietState.reset();

        // Release handles to this server and client server
        m_referenceUnelevated.reset();
        m_referenceElevated.reset();
    }

    UnelevatedServerReference m_referenceUnelevated;
    ElevatedServerReference m_referenceElevated;

    QuietState::unique_quietwindowclose_call m_quietState{ false };
    Timer m_timer;
    std::mutex m_mutex;
};


