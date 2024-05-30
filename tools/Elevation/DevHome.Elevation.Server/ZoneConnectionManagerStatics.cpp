// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <pch.h>

#include <chrono>
#include <map>
#include <memory>
#include <mutex>

#include <wrl/client.h>
#include <wrl/implements.h>
#include <wrl/module.h>

#include <wil/registry.h>
#include <wil/resource.h>
#include <wil/result_macros.h>
#include <wil/win32_helpers.h>
#include <wil/winrt.h>

#include "Utility.h"
#include "DevHome.Elevation.h"

// std::mutex g_mutex;


namespace ABI::DevHome::Elevation
{
    class ElevationVoucherManagerStatics WrlFinal :
        public Microsoft::WRL::AgileActivationFactory<
            Microsoft::WRL::Implements<IElevationVoucherManagerStatics>>
    {
        InspectableClassStatic(RuntimeClass_DevHome_Elevation_ElevationVoucherManager, BaseTrust);

    public:
        STDMETHODIMP ActivateInstance(_COM_Outptr_ IInspectable**) noexcept
        {
            // Disallow activation - must use GetSingleton()
            return E_NOTIMPL;
        }

        STDMETHODIMP ActivateVoucher(
            /* [in] */ IElevationVoucher* voucher,
            /* [in] */ ABI::Windows::Foundation::TimeSpan validDuration) noexcept try
        {
            // This method must called from an elevated process
            
            // todo:jw ensure client caller is elevated

            // Get client mandatory label
            LONG clientLabel;
            {
                // Get calling process handle
                auto revert = wil::CoImpersonateClient();
            
                wil::unique_handle clientToken;
                THROW_IF_WIN32_BOOL_FALSE(OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &clientToken));

                clientLabel = GetTokenMandatoryLabel(clientToken.get());
            }

            // Get our mandatory label
            auto ourLabel = GetTokenMandatoryLabel(GetCurrentProcessToken());
            
            // Only activate a voucher if the requester is at least as elevated as us!
            THROW_HR_IF(E_ACCESSDENIED, clientLabel < ourLabel);

            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP ClaimVoucher(
            /* [in] */ ElevationZone zone,
            /* [out, retval] */ IElevationVoucher** result) noexcept try
        {
            auto revert = wil::CoImpersonateClient();

        }
        CATCH_RETURN()


    private:
        std::mutex m_mutex;
        std::map<std::wstring, std::tuple<unsigned int, ABI::Windows::Foundation::DateTime, ABI::DevHome::Elevation::Zone>> m_preparedConnections;
    };

    ActivatableStaticOnlyFactory(ZoneConnectionManagerStatics);
}




namespace ABI::DevHome::Elevation
{
    class ElevationZoneA :
        public Microsoft::WRL::RuntimeClass<
            Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRt>,
            IElevationZoneA,
            Microsoft::WRL::FtmBase>
    {
        InspectableClass(RuntimeClass_DevHome_Elevation_ElevationZoneA, BaseTrust);

    public:
        STDMETHODIMP RuntimeClassInitialize() noexcept
        {
            return S_OK;
        }

        STDMETHODIMP GetSomething(_Out_ unsigned int* result) noexcept
        {
            // try get registry key to hklm/software/microsoft/windows/currentversion/devhome/quietbackgroundprocesses
            if (auto durationOverride = try_get_registry_value_dword(HKEY_LOCAL_MACHINE, LR"(Software\Microsoft\Windows\CurrentVersion\DevHome\QuietBackgroundProcesses)", L"Duration"))
            {
                *result = (unsigned int)std::chrono::seconds(durationOverride.value()).count();
            }
            *result = 1111;
            return S_OK;
        }
    };

    ActivatableClass(ElevationZoneA);
}