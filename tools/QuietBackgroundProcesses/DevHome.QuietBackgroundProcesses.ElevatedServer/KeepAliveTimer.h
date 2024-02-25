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

auto make_keep_alive_timer_lambda()
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
    static std::thread GetDiscardThread();
    static void Discard(std::unique_ptr<KeepAliveTimer> timer);

    KeepAliveTimer(std::chrono::seconds seconds) :
        m_timer(seconds, make_keep_alive_timer_lambda())
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

    void disconnect()
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

    //Timer<decltype(make_keep_alive_timer_lambda())> m_timer;
    <decltype(make_keep_alive_timer_lambda())> m_timer;
};


