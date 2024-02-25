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

#include "KeepAliveTimer.h"
#include "QuietState.h"

#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.h"
#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager.h"

constexpr auto DEFAULT_QUIET_DURATION = std::chrono::hours(2);

//std::shared_ptr<KeepAlive> g_keepAlive;
std::mutex g_mutex;
std::unique_ptr<KeepAliveTimer> g_activeTimer;

namespace ABI::DevHome::QuietBackgroundProcesses
{
    class QuietBackgroundProcessesSession :
        public Microsoft::WRL::RuntimeClass<
            Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRt>,
            IQuietBackgroundProcessesSession,
            Microsoft::WRL::FtmBase>
    {
        InspectableClass(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSession, BaseTrust);

    public:
        STDMETHODIMP RuntimeClassInitialize() noexcept
        {
            return S_OK;
        }

        // IQuietBackgroundProcessesSession
        STDMETHODIMP Start(__int64* result) noexcept override try
        {
            auto lock = std::scoped_lock(g_mutex);

            // Discard the active timer
            KeepAliveTimer::Discard(std::move(g_activeTimer));

            std::chrono::seconds duration = DEFAULT_QUIET_DURATION;
            if (auto durationOverride = try_get_registry_value_dword(HKEY_CURRENT_USER, LR"(Software\Microsoft\Windows\CurrentVersion\DevHome\QuietBackgroundProcesses)", L"Duration"))
            {
                duration = std::chrono::seconds(durationOverride.value());
            }

            // Start timer
            //m_factory.detach();
            //m_factory = wil::GetActivationFactory<ABI::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionManagerStatics>(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSessionManager);
            /*
            if (!g_keepAlive)
            {
                //g_keepAlive.reset(new KeepAlive());
                g_keepAlive = std::make_shared<KeepAlive>();
            }
            else
            {
                auto duplicate = g_keepAlive;
                duplicate.
            }
            */

            /*
            KeepAlive keepServersAlive;

                KeepAlive z;
            std::unique_ptr<int> y;
            auto x = [z = std::move(z)]() {
            };

            x();

            */

            //std::unique_ptr<Timer> sdf;
            //new Timer(duration, []() mutable {
            //});

            //Timer::Make(duration)

              // todo:jw think about lock here  auto lock = std::scoped_lock(g_mutex);
            //
            g_activeTimer.reset(new KeepAliveTimer(duration));

            /*
            //std::unique_ptr<Timer> sdf;
            g_activeTimer.reset(new Timer(duration, []() mutable {
                auto lock = std::scoped_lock(g_mutex);
                //m_factory->Release();
                g_quietState.reset();
                // keepServersAlive.~KeepAlive();
            }));
            */

            // Turn on quiet mode
            //todo:jw move to KAT
            //g_activeTimer->m_quietState = QuietState::TurnOn();

            // Return duration for showing countdown
            *result = g_activeTimer->TimeLeftInSeconds();
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP Stop() noexcept override try
        {
            auto lock = std::scoped_lock(g_mutex);

            if (g_activeTimer)
            {
                g_activeTimer->Cancel();
            }
            // Turn off quiet mode
            //g_activeTimer->m_quietState.reset();

            // Detach and destruct the current time window
            KeepAliveTimer::Discard(std::move(g_activeTimer));
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_IsActive(::boolean* value) noexcept override try
        {
            auto lock = std::scoped_lock(g_mutex);
            *value = g_activeTimer->IsActive();
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP get_TimeLeftInSeconds(__int64* value) noexcept override try
        {
            auto lock = std::scoped_lock(g_mutex);
            *value = g_activeTimer->TimeLeftInSeconds();
            return S_OK;
        }
        CATCH_RETURN()
    };

    class QuietBackgroundProcessesSessionStatics WrlFinal :
        public Microsoft::WRL::AgileActivationFactory<
            Microsoft::WRL::Implements<IQuietBackgroundProcessesSessionStatics>>
    {
        InspectableClassStatic(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSession, BaseTrust);

    public:
        STDMETHODIMP ActivateInstance(_COM_Outptr_ IInspectable**) noexcept
        {
            // Disallow activation - must use GetSingleton()
            return E_NOTIMPL;
        }

        // IQuietBackgroundProcessesSessionStatics
        STDMETHODIMP GetSingleton(_COM_Outptr_ IQuietBackgroundProcessesSession** session) noexcept override try
        {
            THROW_IF_FAILED(Microsoft::WRL::MakeAndInitialize<QuietBackgroundProcessesSession>(session));
            return S_OK;
        }
        CATCH_RETURN()
    };

    ActivatableClassWithFactory(QuietBackgroundProcessesSession, QuietBackgroundProcessesSessionStatics);
}
