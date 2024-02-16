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
        if (IsDebuggerPresent())
        {
            DebugBreak();
        }
        return S_OK;
    }

    // IServiceHostComponent
    HRESULT QuietBackgroundProcessesSessionManager::GetInt(int* result) noexcept
    {
        *result = 93;
        if (IsDebuggerPresent())
        {
            DebugBreak();
        }
        //return E_NOTIMPL;
        return S_OK;
    }
    //}





_Use_decl_annotations_
        IFACEMETHODIMP
        QuietBackgroundProcessesSessionManagerStatics::GetSession(DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSession**) noexcept //try
    {
        if (IsDebuggerPresent())
        {
            DebugBreak();
        }
        return S_OK;
    }
    //CATCH_RETURN()


_Use_decl_annotations_
        IFACEMETHODIMP
        QuietBackgroundProcessesSessionManagerStatics::TryGetSession(DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSession**) noexcept //try
    {
        if (IsDebuggerPresent())
        {
            DebugBreak();
        }
        return S_OK;
    }
    //CATCH_RETURN()