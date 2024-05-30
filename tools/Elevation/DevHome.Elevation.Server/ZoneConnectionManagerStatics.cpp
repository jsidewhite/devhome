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
            /* [in] */ ABI::Windows::Foundation::TimeSpan /* validDuration*/) noexcept
        try
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

            // Add to m_activatedVouchers
            HSTRING hstrVoucherName;
            THROW_IF_FAILED(voucher->get_VoucherName(&hstrVoucherName));
            auto voucherName = std::wstring(WindowsGetStringRawBuffer(hstrVoucherName, nullptr));
            {
                std::scoped_lock lock(m_mutex);
                m_activatedVouchers.emplace(voucherName, voucher);
            }

            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP ClaimVoucher(
            /* [in] */ HSTRING hstrVoucherName,
            /* [out, retval] */ IElevationVoucher** result) noexcept try
        {
            // Find voucher in m_activatedVouchers
            auto voucherName = std::wstring(WindowsGetStringRawBuffer(hstrVoucherName, nullptr));

            {
                std::scoped_lock lock(m_mutex);
                auto it = m_activatedVouchers.find(voucherName);
                THROW_HR_IF(HRESULT_FROM_WIN32(ERROR_NOT_FOUND), it == m_activatedVouchers.end());

                auto voucherPointer = it->second.get();

                // Ensure client process matches what's stored in the voucher
                {
                    // Get calling process handle
                    auto revert = wil::CoImpersonateClient();

                    wil::unique_process_handle clientProcess;
                    DWORD clientPid = GetProcessId(clientProcess.get());

                    uint32_t voucherProcessId;
                    THROW_IF_FAILED(voucherPointer->get_ProcessId(&voucherProcessId));

                    if (clientPid != voucherProcessId)
                    {
                        THROW_HR(E_ACCESSDENIED);
                    }


                    //todo:jw time match

                }


                //*result = it->second.get();
                auto voucher = std::move(it->second);

                // Remove from m_activatedVouchers
                m_activatedVouchers.erase(it);

                *result = voucher.detach();
            }


            return S_OK;
        }
        CATCH_RETURN()


    private:
        std::mutex m_mutex;
        std::map<std::wstring, wil::com_ptr<ABI::DevHome::Elevation::IElevationVoucher>> m_activatedVouchers;
    };

    ActivatableStaticOnlyFactory(ElevationVoucherManagerStatics);
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