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

#include <wil/resource.h>
#include <wil/result_macros.h>
#include <wil/win32_helpers.h>
#include <wil/winrt.h>

#include "DevHome.Elevation.h"

// std::mutex g_mutex;

namespace ABI::DevHome::Elevation
{
    class ZoneConnectionManagerStatics WrlFinal :
        public Microsoft::WRL::AgileActivationFactory<
            Microsoft::WRL::Implements<IZoneConnectionManagerStatics>>
    {
        InspectableClassStatic(RuntimeClass_DevHome_Elevation_ZoneConnectionManager, BaseTrust);

    public:
        STDMETHODIMP ActivateInstance(_COM_Outptr_ IInspectable**) noexcept
        {
            // Disallow activation - must use GetSingleton()
            return E_NOTIMPL;
        }

        // This method must called from an elevated process
        STDMETHODIMP PrepareConnection(
                /* [in] */ unsigned int pid,
                /* [in] */ ABI::Windows::Foundation::DateTime processCreateTime,
                /* [in] */ ABI::DevHome::Elevation::Zone name,
                /* [out, retval] */ HSTRING* result) noexcept try
        {
            // Create md5 hash of the 3 connection parameters
            std::wstring connectionId = std::to_wstring(pid) + L"_" + std::to_wstring(processCreateTime.UniversalTime) + L"_" + std::to_wstring(static_cast<int>(name));

            // Remember the connection in a std::map
            std::scoped_lock<std::mutex> lock(m_mutex);
            m_preparedConnections[connectionId] = std::make_tuple(pid, processCreateTime, name);

            // return the connectionId
            Microsoft::WRL::Wrappers::HString str;
            str.Set(connectionId.c_str());
            *result = str.Detach();

            return S_OK;
        }
        CATCH_RETURN()

        STDMETHODIMP OpenConnection(
                /* [in] */ HSTRING connectionId,
                /* [out, retval] */ ABI::DevHome::Elevation::IZoneConnection** result) noexcept try
        {
            std::wstring connectionIdStr = WindowsGetStringRawBuffer(connectionId, nullptr);

            // Find the connection in the std::map
            std::scoped_lock<std::mutex> lock(m_mutex);
            auto it = m_preparedConnections.find(connectionIdStr);
            if (it == m_preparedConnections.end())
            {
                return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
            }

            // Create a ZoneConnection object
            wil::com_ptr<ZoneA> zoneConnection;
            RETURN_IF_FAILED(Microsoft::WRL::MakeAndInitialize<ZoneA>(&zoneConnection));

            auto iZoneConnection = zoneConnection.query<IZoneConnection>();

            // Return the ZoneConnection object
            *result = iZoneConnection.detach();
            return S_OK;
        }
        CATCH_RETURN()

    private:
        std::mutex m_mutex;
        std::map<std::wstring, std::tuple<unsigned int, ABI::Windows::Foundation::DateTime, ABI::DevHome::Elevation::Zone>> m_preparedConnections;
    };

    //ActivatableStaticOnlyFactory(ZoneConnectionManagerStatics);
}
