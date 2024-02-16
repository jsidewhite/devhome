// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <windows.h>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/implements.h>
#include <wrl/module.h>
#include <wil/com.h>
#include <wil/result_macros.h>
#include <wil/token_helpers.h>
#include <wil/win32_helpers.h>
#include <wil/winrt.h>

#include "..\tools\QuietBackgroundProcesses\DevHome.QuietBackgroundProcesses.ElevatedServer\DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager_h.h"



int main()
{
    
    using namespace Microsoft;
    using namespace Microsoft::WRL::Wrappers;

    auto unique_rouninitialize_call = wil::RoInitialize();

    //ComPtr<IUriRuntimeClassFactory> uriFactory;
    //HRESULT hr = WRL::GetActivationFactory(HStringReference(RuntimeClass_Windows_Foundation_Uri).Get(), &uriFactory);
    //auto ifac = wil::GetActivationFactory<ABI::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionManager>(RuntimeClass_DevHome_QuietBackgroundProcesses_QuietBackgroundProcessesSessionManager);
    //auto ifac = wil::GetActivationFactory<ABI::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionManager>(L"DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager");
    //auto ifac = wil::GetActivationFactory<IUnknown>(L"DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager");

    //void* f;
    //THROW_IF_FAILED(RoGetActivationFactory(HStringReference(L"DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager").Get(), ));
    wil::ActivateInstance<IInspectable>(L"DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSessionManager");

    

    std::cout << "Hello World!\n";
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
