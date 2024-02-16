#pragma once

//#include <pch.h>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/implements.h>
#include <wrl/module.h>

//#include <Holographic.SI.HotKeyDispatcher.h>
//#include "HotKeys.h"
//#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession_h.h"
#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager.h"

//namespace Windows::Internal::Shell::Holographic::Infrastructure::implementation
//{

class QuietBackgroundProcessesSessionManager :
    public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRt>,
        DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionManager,
        Microsoft::WRL::FtmBase>
{
    InspectableClass(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSessionManager, FullTrust);

public:
    HRESULT RuntimeClassInitialize() noexcept;

    // IQuietBackgroundProcessesSessionManager
    IFACEMETHODIMP GetInt(int* result) noexcept override;
};
//}


class QuietBackgroundProcessesSessionManagerStatics WrlFinal :
    public Microsoft::WRL::AgileActivationFactory<
        Microsoft::WRL::Implements<DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionManagerStatics>>
{
    InspectableClassStatic(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSessionManager, FullTrust);

public:
    // IQuietBackgroundProcessesSessionManagerStatics
    IFACEMETHODIMP GetSession(_COM_Outptr_ DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSession** value) noexcept override;
    IFACEMETHODIMP TryGetSession(_COM_Outptr_opt_ DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSession** value) noexcept override;
};