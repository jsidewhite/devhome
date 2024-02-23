#include <pch.h>

#include <mutex>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/implements.h>
#include <wrl/module.h>
#include <wil/winrt.h>

#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.h"
#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager.h"

/*
template <typename InterfaceT>
struct weakreference
{
    wil::com_ptr<InterfaceT> m_strongReference;

    wil::com_ptr<InterfaceT> resolve()
    {
        // lock
        if (m_strongReference)
        {
            return m_strongReference;
        }
    }
};
*/

template<typename InterfaceT>
struct weakreference
{
    wil::com_ptr<InterfaceT> m_strongReference;

    wil::com_ptr<InterfaceT> resolve()
    {
        // lock
        if (m_strongReference)
        {
            return m_strongReference;
        }
    }
};

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
        STDMETHODIMP InvalidateSessionReference() noexcept override try
        {
            auto lock = std::scoped_lock(m_mutex);

            m_sessionMakeshiftWeakReference.reset();
            return S_OK;
        }
        CATCH_RETURN()

        // IQuietBackgroundProcessesSessionManagerStatics
        STDMETHODIMP GetSession(_COM_Outptr_ IQuietBackgroundProcessesSession** session) noexcept override try
        {
            auto lock = std::scoped_lock(m_mutex);

            auto factory = wil::GetActivationFactory<IQuietBackgroundProcessesSessionStatics>(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSession);
            wil::com_ptr<IQuietBackgroundProcessesSession> instance;
            THROW_IF_FAILED(factory->GetSingleton(&instance));

            // Store a 'weak ref' to the session
            m_sessionMakeshiftWeakReference = instance;

            *session = instance.detach();
            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP TryGetSession(_COM_Outptr_opt_ IQuietBackgroundProcessesSession** session) noexcept override try
        {
            auto lock = std::scoped_lock(m_mutex);

            if (m_sessionMakeshiftWeakReference)
            {
                m_sessionMakeshiftWeakReference.copy_to(session);
                return S_OK;
            }

            *session = nullptr;
            return S_OK;
        }
        CATCH_RETURN()

        // IActivationFactory method
        STDMETHODIMP ActivateInstance(_COM_Outptr_ IInspectable** ppvObject) noexcept try
        {
            THROW_IF_FAILED(Microsoft::WRL::MakeAndInitialize<QuietBackgroundProcessesSessionManager>(ppvObject));
            return S_OK;
        }
        CATCH_RETURN()

    private:
        bool i{};
        bool initd{};
        std::mutex m_mutex;

        // The weak reference will provides a way to interrogate our elevated server process for aliveness.
        //
        // (Ultimately this allows existing clients to reconnect to a running elevated server without the risk of
        // launching a new instance [thereby showing a UAC prompt] in case existing elevated server expired.)
        wil::com_weak_ref m_weakSessionReference;
        wil::com_ptr<IQuietBackgroundProcessesSession> m_sessionMakeshiftWeakReference;
    };

    ActivatableClassWithFactory(QuietBackgroundProcessesSessionManager, QuietBackgroundProcessesSessionManagerStatics);
}
