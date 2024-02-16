#include <pch.h>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/implements.h>
#include <wrl/module.h>

//#include <Holographic.SI.HotKeyDispatcher.h>
//#include "HotKeys.h"
//#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager.h"
#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession_h.h"
#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager_h.h"

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
            if (IsDebuggerPresent())
            {
                DebugBreak();
            }
            return S_OK;
        }

        // IQuietBackgroundProcessesSessionManager
        STDMETHODIMP GetInt(int* result) noexcept override
        {
            if (IsDebuggerPresent())
            {
                DebugBreak();
            }
            *result = 34;
            return S_OK;
        }
    };
    //}

    
    class QuietBackgroundProcessesSessionManagerStatics WrlFinal :
        public Microsoft::WRL::AgileActivationFactory<
            Microsoft::WRL::Implements<IQuietBackgroundProcessesSessionManagerStatics>>
    {
        InspectableClassStatic(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSessionManager, BaseTrust);

    public:
        // IQuietBackgroundProcessesSessionManagerStatics
        IFACEMETHODIMP GetSession(_COM_Outptr_ IQuietBackgroundProcessesSession** value) noexcept override
        {
            *value = nullptr;
            return S_OK;
        }
        IFACEMETHODIMP TryGetSession(_COM_Outptr_opt_ IQuietBackgroundProcessesSession** value) noexcept override
        {
            *value = nullptr;
            return S_OK;
        }
    };


    ActivatableClass(QuietBackgroundProcessesSessionManager);
}
