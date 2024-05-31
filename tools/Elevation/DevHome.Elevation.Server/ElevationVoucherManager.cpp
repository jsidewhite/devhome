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

#include "Helpers.h"
#include "Utility.h"
#include "DevHome.Elevation.h"

MIDL_INTERFACE("68c6a1b9-de39-42c3-8d28-bf40a5126541")
ICallingProcessInfo : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE OpenCallerProcessHandle(
        DWORD desiredAccess,
        /* [annotation][out] */
        _Out_ HANDLE * callerPocessHandle) = 0;
};

static DWORD GetCallingProcessPid()
{
    wil::unique_handle callingProcessHandle;
    wil::unique_handle impersonationToken;
    Microsoft::WRL::ComPtr<ICallingProcessInfo> callingProcessInfo;
    THROW_IF_FAILED(CoGetCallContext(IID_PPV_ARGS(&callingProcessInfo)));
    THROW_IF_FAILED(callingProcessInfo->OpenCallerProcessHandle(PROCESS_QUERY_LIMITED_INFORMATION, callingProcessHandle.addressof()));
    return GetProcessId(callingProcessHandle.get());
}

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
            return E_NOTIMPL;
        }

        STDMETHODIMP ActivateVoucher(
            /* [in] */ IElevationVoucher* voucher,
            /* [in] */ ABI::Windows::Foundation::TimeSpan /* validDuration*/) noexcept
        try
        {
            // This method must called from an elevated process
            // (Or rather, we'll only allow the voucher to be activated if the caller is as elevated as us.)

            // Get client mandatory label
            auto clientLabel = [&]()
            {
                // Get calling process handle
                auto revert = wil::CoImpersonateClient();
            
                wil::unique_handle clientToken;
                THROW_IF_WIN32_BOOL_FALSE(OpenThreadToken(GetCurrentThread(), TOKEN_QUERY, TRUE, &clientToken));

                return GetTokenMandatoryLabel(clientToken.get());
            }();

            // Get our mandatory label
            auto ourLabel = GetTokenMandatoryLabel(GetCurrentProcessToken());
            
            // Only activate a voucher if the requester is at least as elevated as us!
            THROW_HR_IF(E_ACCESSDENIED, clientLabel < ourLabel);

            // Save voucher for a period of time
            HSTRING hstrVoucherName;
            THROW_IF_FAILED(voucher->get_VoucherName(&hstrVoucherName));
            auto voucherName = std::wstring(WindowsGetStringRawBuffer(hstrVoucherName, nullptr));
            {
                std::scoped_lock lock(m_mutex);
                m_activatedVouchers.emplace(voucherName, voucher);
            }

            // Delete voucher after 10 seconds
            auto th = std::thread([voucherName, this]()
            {
                std::this_thread::sleep_for(std::chrono::seconds(10));
                {
                    std::scoped_lock lock(m_mutex);
                    m_activatedVouchers.erase(voucherName);
                }
            });

            th.detach();

            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP ClaimVoucher(
            /* [in] */ HSTRING hstrVoucherName,
            /* [out, retval] */ IElevationVoucher** result) noexcept try
        {
            // Find voucher in m_activatedVouchers
            auto voucherName = std::wstring(WindowsGetStringRawBuffer(hstrVoucherName, nullptr));

            std::scoped_lock lock(m_mutex);
            auto it = m_activatedVouchers.find(voucherName);
            THROW_HR_IF(HRESULT_FROM_WIN32(ERROR_NOT_FOUND), it == m_activatedVouchers.end());

            auto voucherPointer = it->second.get();

            // Get voucher process pid
            uint32_t voucherProcessId;
            THROW_IF_FAILED(voucherPointer->get_ProcessId(&voucherProcessId));

            // Get calling process pid
            DWORD callingProcessPid = GetCallingProcessPid();

            // Ensure client process matches what's stored in the voucher
            if (callingProcessPid != voucherProcessId)
            {
                THROW_HR(E_ACCESSDENIED);
            }

            // Stop tracking the voucher and return it to unelevated client
            auto voucher = std::move(it->second);
            m_activatedVouchers.erase(it);
            *result = voucher.detach();

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
    class ElevationVoucher :
        public Microsoft::WRL::RuntimeClass<
            Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::WinRt>,
            IElevationVoucher,
            Microsoft::WRL::FtmBase>
    {
        InspectableClass(RuntimeClass_DevHome_Elevation_ElevationVoucher, BaseTrust);

    public:
        STDMETHODIMP RuntimeClassInitialize(HSTRING voucherName, ElevationZone zoneId, uint32_t processId, ABI::Windows::Foundation::DateTime processCreateTime) noexcept
        {
            m_voucherName = std::wstring(WindowsGetStringRawBuffer(voucherName, nullptr));
            m_zoneId = zoneId;
            m_processId = processId;
            m_processCreateTime = processCreateTime;
            return S_OK;
        }

        STDMETHODIMP get_VoucherName(_Out_ HSTRING* result) noexcept
        {
            Microsoft::WRL::Wrappers::HStringReference(m_voucherName.c_str()).CopyTo(result);
            return S_OK;
        }

        STDMETHODIMP get_ZoneId(_Out_ ElevationZone* result) noexcept
        {
            *result = m_zoneId;
            return S_OK;
        }

        STDMETHODIMP get_ProcessId(_Out_ unsigned int* result) noexcept
        {
            *result = m_processId;
            return S_OK;
        }

        STDMETHODIMP get_ProcessCreateTime(_Out_ Windows::Foundation::DateTime* result) noexcept
        {
            *result = m_processCreateTime;
            return S_OK;
        }

        STDMETHODIMP Redeem(_COM_Outptr_ IElevationZone** result) noexcept
        {
            THROW_IF_FAILED(MakeElevationZone(m_zoneId, result));
            return S_OK;
        }

    private:
        std::wstring m_voucherName;
        ElevationZone m_zoneId;
        uint32_t m_processId;
        ABI::Windows::Foundation::DateTime m_processCreateTime;
    };

    class ElevationVoucherFactory WrlFinal :
        public Microsoft::WRL::AgileActivationFactory<
            Microsoft::WRL::Implements<IElevationVoucherFactory>>
    {
        InspectableClassStatic(RuntimeClass_DevHome_Elevation_ElevationVoucher, BaseTrust);

    public:
        
        STDMETHODIMP ActivateInstance(_COM_Outptr_ IInspectable**) noexcept
        {
            // Disallow activation - must use GetSingleton()
            return E_NOTIMPL;
        }

        STDMETHODIMP CreateInstance(
            /* [in] */ HSTRING voucherName,
            /* [in] */ ElevationZone zoneId,
            /* [in] */ UINT32 processId,
            /* [in] */ ABI::Windows::Foundation::DateTime processCreateTime,
            /* [out, retval] */ IElevationVoucher** result) noexcept
        {
            auto voucher = Microsoft::WRL::Make<ElevationVoucher>();
            THROW_IF_FAILED(voucher->RuntimeClassInitialize(voucherName, zoneId, processId, processCreateTime));
            *result = voucher.Detach();
            return S_OK;
        }
    };

    ActivatableClassWithFactory(ElevationVoucher, ElevationVoucherFactory);
}
