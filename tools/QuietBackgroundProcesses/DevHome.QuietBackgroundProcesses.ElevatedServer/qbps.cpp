#include <pch.h>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/implements.h>
#include <wrl/module.h>
#include <wil/winrt.h>

//#include <Holographic.SI.HotKeyDispatcher.h>
//#include "HotKeys.h"
//#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession_h.h"
//#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.h"

#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.h"

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

        STDMETHODIMP Start(__int64* result) noexcept override
        {
            if (IsDebuggerPresent())
            {
                //DebugBreak();
            }
            *result = 34;
            return S_OK;
        }

        STDMETHODIMP Stop(void) noexcept override
        {
            if (IsDebuggerPresent())
            {
                //DebugBreak();
            }
            return S_OK;
        }

        STDMETHODIMP get_IsActive(::boolean* value) noexcept override
        {
            if (IsDebuggerPresent())
            {
                //DebugBreak();
            }
            *value = true;
            return S_OK;
        }

        STDMETHODIMP get_TimeLeftInSeconds(__int64* value) noexcept override
        {
            if (IsDebuggerPresent())
            {
                //DebugBreak();
            }
            *value = 234;
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
        STDMETHODIMP GetSingleton(_COM_Outptr_ IQuietBackgroundProcessesSession** value) noexcept override
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
            THROW_IF_FAILED(Microsoft::WRL::MakeAndInitialize<QuietBackgroundProcessesSession>(value));
            return S_OK;
        }
        CATCH_RETURN()

        // IActivationFactory method
        STDMETHODIMP ActivateInstance(_COM_Outptr_ IInspectable** ppvObject) noexcept
        {
            //return Microsoft::WRL::MakeAndInitialize<QuietBackgroundProcessesSessionManager>(ppvObject);
            return E_NOTIMPL;
        }

    private:
        bool initd{};
    };

    ActivatableClassWithFactory(QuietBackgroundProcessesSession, QuietBackgroundProcessesSessionStatics);
}
