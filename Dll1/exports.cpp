#include "pch.h"

#include <combaseapi.h>

#include <wil/result_macros.h>

extern "C" HRESULT RegisterObjects() noexcept try
{
    THROW_IF_FAILED(CoRegisterClassObject(
        riid,
        spFactoryWrapper->CastToUnknown(),
        CLSCTX_INPROC_SERVER,
        REGCLS_MULTIPLEUSE | REGCLS_AGILE,
        &dwToken));
}
CATCH_RETURN()
