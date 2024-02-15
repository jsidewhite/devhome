#pragma once

#include <pch.h>

//#include "DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession_h.h"
#include "QuietBackgroundProcessesSession.h"

//namespace Windows::Internal::Shell::Holographic::Infrastructure::implementation
//{

HRESULT QuietBackgroundProcessesSession::RuntimeClassInitialize() noexcept
{
    return E_NOTIMPL;
}

HRESULT QuietBackgroundProcessesSession::Start(__int64* result) noexcept
{
    *result = 23423;
    return E_NOTIMPL;
}

HRESULT QuietBackgroundProcessesSession::Stop(void) noexcept
{
    return E_NOTIMPL;
}

HRESULT QuietBackgroundProcessesSession::get_IsActive(::boolean* value) noexcept
{
    *value = false;
    return E_NOTIMPL;
}

HRESULT QuietBackgroundProcessesSession::get_TimeLeftInSeconds(__int64* value) noexcept
{
    *value = 65;
    return E_NOTIMPL;
}
