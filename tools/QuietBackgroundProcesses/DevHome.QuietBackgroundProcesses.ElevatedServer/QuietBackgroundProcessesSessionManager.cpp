#pragma once

#include <pch.h>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/implements.h>
#include <wrl/module.h>

//#include <Holographic.SI.HotKeyDispatcher.h>
//#include "HotKeys.h"
//#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager_h.h"
#include "QuietBackgroundProcessesSessionManager.h"

//namespace Windows::Internal::Shell::Holographic::Infrastructure::implementation
//{

    HRESULT QuietBackgroundProcessesSessionManager::RuntimeClassInitialize() noexcept
{
        return S_OK;
    }

    // IServiceHostComponent
    HRESULT QuietBackgroundProcessesSessionManager::GetInt(int* result) noexcept
    {
        * result = 93;
        //return E_NOTIMPL;
        return S_OK;
    }
    //}
