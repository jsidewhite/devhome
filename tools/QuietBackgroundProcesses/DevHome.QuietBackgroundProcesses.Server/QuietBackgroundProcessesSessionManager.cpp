// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/implements.h>
#include <wrl/module.h>
#include <wil/winrt.h>

#include "DevHome.QuietBackgroundProcesses.h"

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
        STDMETHODIMP ActivateInstance(_Outptr_result_nullonfailure_ IInspectable** ppvObject) noexcept
        try
        {
            THROW_IF_FAILED(Microsoft::WRL::MakeAndInitialize<QuietBackgroundProcessesSessionManager>(ppvObject));
            return S_OK;
        }
        CATCH_RETURN()

        // IQuietBackgroundProcessesSessionManagerStatics
        STDMETHODIMP IsFeaturePresent(_Out_ boolean* isPresent) noexcept override try
        {
            *isPresent = true;
            return S_OK;
        }
        CATCH_RETURN();

        STDMETHODIMP get_GetInt(unsigned __int32* result) noexcept override
        try
        {
            *result = 37;
            return S_OK;
        }
        CATCH_RETURN()

    };

    ActivatableClassWithFactory(QuietBackgroundProcessesSessionManager, QuietBackgroundProcessesSessionManagerStatics);
}
