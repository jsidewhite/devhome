#include <pch.h>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/implements.h>
#include <wrl/module.h>
#include <wil/winrt.h>

#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.h"
#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager.h"

namespace ABI::DevHome::QuietBackgroundProcesses
{
    class QuietBackgroundProcessesSessionManager :
        public Microsoft::WRL::RuntimeClass<
            Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRt>,
            IQuietBackgroundProcessesSessionManager,
            Microsoft::WRL::FtmBase>
    {
        InspectableClass(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSessionManager, BaseTrust);

    public:
        STDMETHODIMP RuntimeClassInitialize() noexcept
        {
            return S_OK;
        }
    };

    class QuietBackgroundProcessesSessionManagerStatics WrlFinal :
        public Microsoft::WRL::AgileActivationFactory<
            Microsoft::WRL::Implements<IQuietBackgroundProcessesSessionManagerStatics>>
    {
        InspectableClassStatic(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSessionManager, BaseTrust);

    public:
        // IQuietBackgroundProcessesSessionManagerStatics
        STDMETHODIMP GetSession(_COM_Outptr_ IQuietBackgroundProcessesSession** session) noexcept override try
        {
            auto y = wil::GetActivationFactory<IQuietBackgroundProcessesSessionStatics>(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSession);
            wil::com_ptr<IQuietBackgroundProcessesSession> instance;
            THROW_IF_FAILED(y->GetSingleton(&instance));
            initd = true;
            *session = instance.detach();
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP TryGetSession(_COM_Outptr_opt_ IQuietBackgroundProcessesSession** session) noexcept override
        {
            if (!initd)
            {
                //*session = nullptr;
                //return S_OK;
            }
            //auto x = wil::ActivateInstance<IQuietBackgroundProcessesSession>(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSession);
            //*value = x.detach();
            auto y = wil::GetActivationFactory<IQuietBackgroundProcessesSessionStatics>(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSession);
            wil::com_ptr<IQuietBackgroundProcessesSession> instance;
            THROW_IF_FAILED(y->GetSingleton(&instance));
            initd = true;
            *session = instance.detach();
            return S_OK;
        }

        // IActivationFactory method
        STDMETHODIMP ActivateInstance(_COM_Outptr_ IInspectable** ppvObject) noexcept try
        {
            //return Microsoft::WRL::MakeAndInitialize<QuietBackgroundProcessesSessionManager>(ppvObject);
            THROW_IF_FAILED(Microsoft::WRL::MakeAndInitialize<QuietBackgroundProcessesSessionManager>(ppvObject));
            return S_OK;
        }
        CATCH_RETURN()

    private:
        bool initd{};
    };

    ActivatableClassWithFactory(QuietBackgroundProcessesSessionManager, QuietBackgroundProcessesSessionManagerStatics);
}
