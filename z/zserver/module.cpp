//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
//
//*********************************************************

#include "pch.h"

using namespace ABI::Windows::ApplicationModel::Core;
using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

class ExeServerGetActivationFactory WrlFinal : public RuntimeClass<IGetActivationFactory, FtmBase>
{
public:
    STDMETHODIMP GetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ IInspectable** factory)
    {
        *factory = nullptr;
        ComPtr<IActivationFactory> activationFactory;
        HRESULT hr = Module<InProc>::GetModule().GetActivationFactory(activatableClassId, &activationFactory);
        if (SUCCEEDED(hr))
        {
            *factory = activationFactory.Detach();
        }
        return hr;
    }
};

int CALLBACK WinMain(_In_  HINSTANCE, _In_  HINSTANCE, _In_  LPSTR, _In_  int)
{
    /*
    Microsoft::WRL::Wrappers::RoInitializeWrapper roInit(RO_INIT_MULTITHREADED);
    if (FAILED(roInit)) return 0;


    ComPtr<ICoreApplication> coreApp;
    if (FAILED(GetActivationFactory(HStringReference(RuntimeClass_Windows_ApplicationModel_Core_CoreApplication).Get(), &coreApp))) return 0;

    auto activationFactory = Make<ExeServerGetActivationFactory>();
    if (!activationFactory) return 0;

    coreApp->RunWithActivationFactories(activationFactory.Get());
    */

    Microsoft::WRL::Wrappers::RoInitializeWrapper roInit(RO_INIT_MULTITHREADED);
    HStringReference acidsRefs[1] = { HStringReference(L"zruncomponent.Class") };
    HSTRING acids[1] = { acidsRefs[0].Get()};


    RO_REGISTRATION_COOKIE winrtRegistrationCookie = NULL;

    /*
    auto cleanup = wil::scope_exit([&] {
        if (winrtRegistrationCookie)
        {
            RoRevokeActivationFactories(winrtRegistrationCookie);
        }
        CoUninitialize();
    });
    */


    PFNGETACTIVATIONFACTORY perAppRTBWinrtAAAUCallback = [](HSTRING, IActivationFactory** ) -> HRESULT {
        return S_OK;
    };

    PFNGETACTIVATIONFACTORY callbacks[1] = { perAppRTBWinrtAAAUCallback };

    HRESULT hr = RoRegisterActivationFactories(acids, callbacks, ARRAYSIZE(callbacks), &winrtRegistrationCookie);
    //RETURN_IF_FAILED(RoRegisterActivationFactories(acids, callbacks, ARRAYSIZE(callbacks), &winrtRegistrationCookie));
    if (FAILED(hr))
    {
        //throw 1;
        Sleep(20000);
    }

    Sleep(20000);


    RoRevokeActivationFactories(winrtRegistrationCookie);


    return 0;
}
