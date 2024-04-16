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

#include "DevHome.QuietBackgroundProcesses.h"

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


// TimedQuietSession is a 2 hour "Quiet background processes" timed window that disables quiet
// mode when the timer expires or when explicitly cancelled.  It keeps also keeps the server alive.
// 
// TimedQuietSession maintains,
//      1. quiet mode signal
//      2. session timer
//      3. handle to session (elevated) server
//      4. handle to manager (unelevated) server
// 
// COM server lifetime & process interaction:
// 
//      Processes:
//          DevHome (client) -> UnelevatedServer (manager) -> ElevatedServer (session)
// 
//      Manager (unelevated server):
//          Role of manager is simply allow client to check if session (elevated server) is alive
//          without risking launching a UAC prompt.  It caches a com proxy interface to session.
// 
//      Session (elevated server):
//          Role of session is to enable/disable quiet mode and keep a timer.
// 
//      Lifetime:
//          - The manager and session keep each other alive.
// 
//          - The session (TimedQuietSession) runs in ElevatedServer and keeps this elevated server
//              alive until timer expiration.
// 
//          - The session (TimedQuietSession) also keeps the manager (in unelevated server) alive until timer expiration;
//              this is only because the manager caches a *strong* handle to the session (elevated server),
//              and there is no way to invalidate the session (proxy handle) in the client if we tore down
//              this session.  (Using IWeakReference does hold a weak reference to the object, but also holds a strong
//              reference against the hosting process's lifetime.)
// 
//      Teardown Sequence:
//          When all session timers (elevated) expire, the manager (unelevated) reference is released -> COM triggers
//          teardown in unelevated server (assuming DevHome is closed), releasing the cached *strong* reference
//          to the session (elevated) -> COM triggers teardown in elevated server.
//
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

        // Start performance recorder
        //THROW_IF_FAILED(Microsoft::WRL::MakeAndInitialize<ABI::DevHome::QuietBackgroundProcesses::PerformanceRecorderEngine>(&m_performanceRecorderEngine));
        ABI::Windows::Foundation::TimeSpan samplingPeriod;
        samplingPeriod.Duration = 1000 * 10000; // 1 second
        THROW_IF_FAILED(m_performanceRecorderEngine->Start(samplingPeriod));
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

    void Cancel(ABI::DevHome::QuietBackgroundProcesses::IProcessPerformanceTable** result)
    {
        auto lock = std::scoped_lock(m_mutex);

        Deactivate(result);
        m_timer->Cancel();

        // Destruct timer on another thread because it's destructor is blocking
        auto destructionThread = std::thread([timer = std::move(m_timer)]() {
            // destruct timer here
        });

        destructionThread.detach();
    }

private:
    void Deactivate(ABI::DevHome::QuietBackgroundProcesses::IProcessPerformanceTable** result = nullptr)
    {
        // Turn off quiet mode
        m_quietState.reset();

        // Stop the performance recorder and write the .csv to disk
        LOG_IF_FAILED(m_performanceRecorderEngine->Stop(result));
        m_performanceRecorderEngine.reset();

        // Release lifetime handles to this elevated server and unelevated client server
        m_unelevatedServer.reset();
        m_elevatedServer.reset();
    }

    UnelevatedServerReference m_unelevatedServer;   // Manager server
    ElevatedServerReference m_elevatedServer;       // Session server (this server)

    QuietState::unique_quietwindowclose_call m_quietState{ false };
    std::unique_ptr<Timer> m_timer;
    wil::com_ptr<ABI::DevHome::QuietBackgroundProcesses::IPerformanceRecorderEngine> m_performanceRecorderEngine;
    std::mutex m_mutex;
};
