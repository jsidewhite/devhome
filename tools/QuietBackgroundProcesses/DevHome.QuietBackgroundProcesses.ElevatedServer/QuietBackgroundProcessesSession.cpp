#include <pch.h>





#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <vector>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/implements.h>
#include <wrl/module.h>

#include <wil/resource.h>
#include <wil/result_macros.h>
#include <wil/win32_helpers.h>
#include <wil/winrt.h>

#include "Timer.h"
#include "QuietState.h"



//#include <Holographic.SI.HotKeyDispatcher.h>
//#include "HotKeys.h"
//#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession_h.h"
//#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.h"

#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.h"

//constexpr auto QUIET_DURATION = std::chrono::hours(2);
constexpr auto QUIET_DURATION = std::chrono::seconds(10);

std::mutex g_mutex;
std::unique_ptr<Timer> g_activeTimer;

QuietState::unique_quietwindowclose_call g_quietState{ false };

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
            if (IsDebuggerPresent())
            {
                //DebugBreak();
            }
            return S_OK;
        }
        /*
        // IWeakReference
        STDMETHODIMP Resolve(_Out_ IInspectable** objectReference) noexcept override
        {
            //static_assert(__is_base_of(IInspectable, T), "Only Windows Runtime interfaces can be resolved by weak reference");
            //return Resolve(__uuidof(T), (IInspectable**)objectReference);
            *objectReference = nullptr;
            return S_OK;
        }
        */

        // IQuietBackgroundProcessesSession
        STDMETHODIMP Start(__int64* result) noexcept override
        {
            auto lock = std::scoped_lock(g_mutex);

            // Discard the active timer
            Timer::Discard(std::move(g_activeTimer));

            // Start timer
            g_activeTimer.reset(new Timer(QUIET_DURATION, []() {
                auto lock = std::scoped_lock(g_mutex);
                g_quietState.reset();
            }));

            // Turn on quiet mode
            g_quietState = QuietState::TurnOn();

            // Return duration for showing countdown
            *result = g_activeTimer->TimeLeftInSeconds();
            return S_OK;
        }

        STDMETHODIMP Stop(void) noexcept override
        {
            auto lock = std::scoped_lock(g_mutex);

            // Turn off quiet mode
            g_quietState.reset();

            // Detach and destruct the current time window
            Timer::Discard(std::move(g_activeTimer));
            return S_OK;
        }

        STDMETHODIMP get_IsActive(::boolean* value) noexcept override
        {
            auto lock = std::scoped_lock(g_mutex);
            *value = (bool)g_quietState;
            return S_OK;
        }

        STDMETHODIMP get_TimeLeftInSeconds(__int64* value) noexcept override
        {
            auto lock = std::scoped_lock(g_mutex);
            if (!g_quietState || !g_activeTimer)
            {
                return 0;
            }

            *value = g_activeTimer->TimeLeftInSeconds();
            return S_OK;
        }
    };

    class QuietBackgroundProcessesSessionStatics WrlFinal :
        public Microsoft::WRL::AgileActivationFactory<
            Microsoft::WRL::Implements<IQuietBackgroundProcessesSessionStatics>>
    {
        InspectableClassStatic(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSession, BaseTrust);

    public:
        // IQuietBackgroundProcessesSessionStatics
        STDMETHODIMP GetSingleton(_COM_Outptr_ IQuietBackgroundProcessesSession** session) noexcept override
        try
        {
            /*
            auto y = wil::GetActivationFactory<IQuietBackgroundProcessesSessionStatics>(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSession);
            wil::com_ptr<IQuietBackgroundProcessesSession> instance;
            THROW_IF_FAILED(y->GetSingleton(&instance));
            initd = true;
            *value = instance.detach();
            return S_OK;
            */
            THROW_IF_FAILED(Microsoft::WRL::MakeAndInitialize<QuietBackgroundProcessesSession>(session));
            return S_OK;
        }
        CATCH_RETURN()

        // IActivationFactory method
        STDMETHODIMP ActivateInstance(_COM_Outptr_ IInspectable**) noexcept
        {
            // Disallow activation - must use GetSingleton()
            return E_NOTIMPL;
        }

    private:
        bool initd{};
    };

    ActivatableClassWithFactory(QuietBackgroundProcessesSession, QuietBackgroundProcessesSessionStatics);
}
