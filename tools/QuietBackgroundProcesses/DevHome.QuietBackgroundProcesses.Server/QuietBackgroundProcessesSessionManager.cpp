#include <pch.h>

#include <mutex>

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
        // IActivationFactory method
        STDMETHODIMP ActivateInstance(_COM_Outptr_ IInspectable** ppvObject) noexcept
        try
        {
            THROW_IF_FAILED(Microsoft::WRL::MakeAndInitialize<QuietBackgroundProcessesSessionManager>(ppvObject));
            return S_OK;
        }
        CATCH_RETURN()

        // IQuietBackgroundProcessesSessionManagerStatics
        STDMETHODIMP GetSession(_COM_Outptr_ IQuietBackgroundProcessesSession** session) noexcept override try
        {
            auto lock = std::scoped_lock(m_mutex);

            if (!m_sessionReference)
            {
                auto factory = wil::GetActivationFactory<IQuietBackgroundProcessesSessionStatics>(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSession);
                THROW_IF_FAILED(factory->GetSingleton(&m_sessionReference));
            }

            m_sessionReference.copy_to(session);
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP TryGetSession(_COM_Outptr_opt_ IQuietBackgroundProcessesSession** session) noexcept override try
        {
            auto lock = std::scoped_lock(m_mutex);
            m_sessionReference.try_copy_to(session);
            return S_OK;
        }
        CATCH_RETURN()

    private:
        std::mutex m_mutex;
        wil::com_ptr<IQuietBackgroundProcessesSession> m_sessionReference;
    };

    ActivatableClassWithFactory(QuietBackgroundProcessesSessionManager, QuietBackgroundProcessesSessionManagerStatics);
}
