#include <pch.h>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/implements.h>
#include <wrl/module.h>

//#include <Holographic.SI.HotKeyDispatcher.h>
//#include "HotKeys.h"
//#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession_h.h"
//#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.h"

#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession_h.h"

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
                DebugBreak();
            }
            return S_OK;
        }

        STDMETHODIMP Start(__int64* result) noexcept override
        {
            if (IsDebuggerPresent())
            {
                DebugBreak();
            }
            *result = 34;
            return S_OK;
        }

        STDMETHODIMP Stop(void) noexcept override
        {
            if (IsDebuggerPresent())
            {
                DebugBreak();
            }
            return S_OK;
        }

        STDMETHODIMP get_IsActive(::boolean* value) noexcept override
        {
            if (IsDebuggerPresent())
            {
                DebugBreak();
            }
            *value = true;
            return S_OK;
        }

        STDMETHODIMP get_TimeLeftInSeconds(__int64* value) noexcept override
        {
            if (IsDebuggerPresent())
            {
                DebugBreak();
            }
            *value = 234;
            return S_OK;
        }
    };

    ActivatableClass(QuietBackgroundProcessesSession);
}
