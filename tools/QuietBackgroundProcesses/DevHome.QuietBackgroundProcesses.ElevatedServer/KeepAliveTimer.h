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
    wil::com_ptr<ABI::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionManagerStatics> m_factory;
    UnelevatedServerReference() :
        m_factory(wil::GetActivationFactory<ABI::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionManagerStatics>(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSessionManager))
    {
    }
};

struct ElevatedServerReference
{
    wrl_server_process_ref m_reference;
};

/*
struct KeepAlive
{
    KeepAlive() = default;

    KeepAlive(KeepAlive&& other) noexcept = default;
    KeepAlive& operator=(KeepAlive&& other) noexcept = default;

    KeepAlive(const KeepAlive&) = delete;
    KeepAlive& operator=(const KeepAlive&) = delete;

    UnelevatedServerReference m_referenceUnelevated;
    ElevatedServerReference m_referenceElevated;
};
*/

//using keep_alive_timer = Timer<decltype(make_keep_alive_timer())> m_;

auto make_keep_alive_timer()
{
    return []() {
        //todo:jw auto lock = std::scoped_lock(g_mutex);
        //m_factory->Release();
        // todo:jw m_quietState.reset();
        // keepServersAlive.~KeepAlive();
    };
}

struct KeepAliveTimer
{
    // Cleanup functions
    static void Discard(std::unique_ptr<KeepAliveTimer> timer);
    static std::thread GetDiscardThread();

    KeepAliveTimer(std::chrono::seconds seconds) :
        m_timer(seconds, make_keep_alive_timer())
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
        /*
                    auto lock = std::scoped_lock(g_mutex);
            if (!g_activeTimer->m_quietState || !g_activeTimer)
            {
                return 0;
            }

            *value = g_activeTimer->TimeLeftInSeconds();
            return S_OK;
            
            */
        return m_timer.TimeLeftInSeconds();
    }

    bool IsActive()
    {
        //todo:jw lock?
        return (bool)m_quietState;
    }

    void Cancel()
    {
        // Turn off quiet mode
        m_quietState.reset();

        //todo:jw
        m_timer.Cancel();
        //reset m_referenceUnelevated
        // reset m_referenceElevated
    }

private:
    UnelevatedServerReference m_referenceUnelevated;
    ElevatedServerReference m_referenceElevated;

    QuietState::unique_quietwindowclose_call m_quietState{ false };

    Timer<decltype(make_keep_alive_timer())> m_timer;
};


