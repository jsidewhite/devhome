#pragma once

//#include <pch.h>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/implements.h>
#include <wrl/module.h>

//#include <Holographic.SI.HotKeyDispatcher.h>
//#include "HotKeys.h"
//#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession_h.h"
#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession.h"

//namespace Windows::Internal::Shell::Holographic::Infrastructure::implementation
//{

class QuietBackgroundProcessesSession:
    public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRt>,
        DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSession,
        Microsoft::WRL::FtmBase>
{
    InspectableClass(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSession, FullTrust);

public:
    HRESULT RuntimeClassInitialize() noexcept;

                IFACEMETHODIMP Start(
        /* [out, retval] */ __int64* result) noexcept override;

    IFACEMETHODIMP Stop(void) noexcept override;

                /* [propget] */ IFACEMETHODIMP get_IsActive(
        /* [out, retval] */ ::boolean* value) noexcept override;

    /* [propget] */ IFACEMETHODIMP get_TimeLeftInSeconds(
        /* [out, retval] */ __int64* value) noexcept override;
};
//}
