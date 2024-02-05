// stl
#include <algorithm>
#include <condition_variable>
#include <chrono>
#include <future>
#include <mutex>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <unordered_set>
#include <vector>

// windows
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include "windows.h"

// wil
#include <wil/com.h>
#include <wrl/implements.h>
#include <wil/resultmacros.h>
#include <wil/resource.h>
#include <wil/result.h>
#include <wil/token_helpers.h>
#include <wil/win32_helpers.h>

// other
#include <shellapi.h>
#include "windows.h"
#include <ActivityCoordinator.h>
#include "werapi.h"

#include "winnls.h"
#include "shobjidl.h"
#include "objbase.h"
#include "objidl.h"
#include "shlguid.h"
#include "propkey.h"
#include "propkeyp.h"

// crm
#include <resourcemanagercrm.h>

// wnf stuff
#include "power.h"
#include "wnfnamesp.h"

// power stuff
#include "ntpoapi_p.h"
#include "powrprof.h" // DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS

// power saver (RM_QUIET_MODE_DATA)
#include "crmtypes.h"

// process stuff
#include <tlhelp32.h>

// battery stuff
#include "poclass.h"


namespace performance
{
    #define NEXT_PROCESS(p) reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>(((PUCHAR)p + p->NextEntryOffset))

    double
    CalcProcessor(
        _In_ ULONGLONG const previousValue,
        _In_ ULONGLONG const currentValue,
        _In_ ULONGLONG const duration,
        _In_ USHORT numberOfCpus)
    {
        if (currentValue > previousValue && duration != 0)
        {
            double temp = (static_cast<double>((currentValue - previousValue) / static_cast<double>(duration))) * 100.0 / numberOfCpus;
            return temp > 100 ? 99.0 : temp;
        }

        return 0;
    }

    HRESULT
    GetCycleTime(_Out_ PULONGLONG timeStampCounter)
    {
        HRESULT hr = S_OK;
        THREAD_CYCLE_TIME_INFORMATION timeInfo;

        if (FAILED(hr = HRESULT_FROM_NT(NtQueryInformationThread(GetCurrentThread(), ThreadCycleTime, &timeInfo, sizeof(timeInfo), NULL))))
        {
            return hr;
        }

        *timeStampCounter = timeInfo.CurrentCycleCount;
        return hr;
    }

    std::optional<SYSTEM_PROCESS_INFORMATION> GetProcessInfo(ULONG pid)
    {
        ULONG performanceInformationLength;
        SYSTEM_PERFORMANCE_INFORMATION performanceInformation;

        // general system performance metrics - not that useful
        THROW_IF_NTSTATUS_FAILED(NtQuerySystemInformation(SystemPerformanceInformation, &performanceInformation, sizeof(performanceInformation), &performanceInformationLength));

        // per-process info
        //SYSTEM_PROCESS_INFORMATION processInfos;
        //ULONG processInfosLength;
        ULONG cbSize = 0;
        NTSTATUS status{};
        //THROW_IF_NTSTATUS_FAILED(NtQuerySystemInformation(SystemProcessInformation, 0, sizeof(cbSize), &cbSize));
        //THROW_IF_NTSTATUS_FAILED

        status = NtQuerySystemInformation(SystemProcessInformation, 0, cbSize, &cbSize);
        //THROW_NTSTATUS(status);
        if (status != STATUS_INFO_LENGTH_MISMATCH)
        {
            THROW_NTSTATUS(status);
        }

        std::vector<BYTE> buffer;
        while (true)
        {
            buffer.resize(cbSize);
            // char* x = new char[cbSize];
            // THROW_IF_NTSTATUS_FAILED(NtQuerySystemInformation(SystemProcessInformation, x, cbSize, &cbSize));
            //THROW_IF_NTSTATUS_FAILED(NtQuerySystemInformation(SystemProcessInformation, buffer.data(), cbSize, &cbSize));
            //THROW_IF_NTSTATUS_FAILED(NtQuerySystemInformation(SystemProcessInformation, &buffer[0], cbSize, &cbSize));
            status = NtQuerySystemInformation(SystemProcessInformation, buffer.data(), cbSize, &cbSize);
            if (status == STATUS_INFO_LENGTH_MISMATCH)
            {
                continue;
            }
            THROW_IF_NTSTATUS_FAILED(status);
            break;
        }

        //log("cbSize ", cbSize);


        SYSTEM_PROCESS_INFORMATION* processInfoNext;
        SYSTEM_PROCESS_INFORMATION* processInfo;
        processInfoNext = reinterpret_cast<PSYSTEM_PROCESS_INFORMATION>((void *)buffer.data());
        do
        {
            processInfo = processInfoNext;
            THROW_HR_IF(E_FAIL, ((ULONG)((BYTE*)processInfo - (BYTE*)(void*)buffer.data()) > cbSize));

            auto currentPid = HandleToULong(processInfo->UniqueProcessId);

            if (currentPid == pid)
            {
                return {*processInfo};
            }

            //hr = this->Update(processInfo, duration, cycleDuration, index, &current, &cycles);
            processInfoNext = NEXT_PROCESS(processInfo);
        } while (processInfoNext != processInfo);

        return std::nullopt;
    }

    //std::vector<double> g_procUsage;
    //std::mutex g_procUsageLock;
    //std::atomic<double> g_procUsage;

    template <typename CallbackT>
    std::thread RunPerformanceMonitor(ULONG pid, CallbackT&& callback)
    {
        // Let's do this on another thread
        auto thread = std::thread([pid, callback = std::move(callback)]()
        {
            ULONGLONG thistimestamp{};
            ULONGLONG thissystemCycle{};

            // hrm
            ULONGLONG previousCycleTime = 0;
            ULONGLONG previousNewTime = 0;

            //double current = 0.0;
            //double cycles = 0.0;
            ULONGLONG timestampEnter;
            ULONGLONG timestampExit;
            ULONGLONG systemCycleEnter;
            ULONGLONG systemCycleExit;
            //SYSTEM_PROCESS_INFORMATION* processInfo;
            //SYSTEM_PROCESS_INFORMATION* processInfoNext;
            ULONGLONG cycleDuration = 0;
            ULONGLONG duration = 0;

            SYSTEM_INFO systemInfo;
            GetSystemInfo(&systemInfo);
            auto numCpus = (short)systemInfo.dwNumberOfProcessors;

            while (true)
            {
                QueryUnbiasedInterruptTime(&timestampEnter);
                performance::GetCycleTime(&systemCycleEnter);
                //nt = NtQuerySystemInformation(SystemProcessInformation, buffer, cbSize, &cbSize);
                //auto processInfo = performance::GetProcessInfo(pid);
                auto maybeProcessInfo = performance::GetProcessInfo(pid);
                if (!maybeProcessInfo)
                {
                    break;
                }
                
                auto processInfo = maybeProcessInfo.value();
                
                QueryUnbiasedInterruptTime(&timestampExit);
                performance::GetCycleTime(&systemCycleExit);

                if (thistimestamp != 0)
                {
                    duration = timestampExit - thistimestamp;
                }

                if (thissystemCycle != 0 && systemCycleExit != 0)
                {
                    cycleDuration = systemCycleExit - thissystemCycle;
                }

                thistimestamp = timestampEnter;
                thissystemCycle = systemCycleEnter;

                //log("process: ", pid);
                //log("processInfo->CycleTime: ", processInfo.CycleTime);

                ULONGLONG newTime = (ULONGLONG)(processInfo.UserTime.QuadPart + processInfo.KernelTime.QuadPart);

                

                double procUsage = performance::CalcProcessor(previousCycleTime, processInfo.CycleTime, cycleDuration, numCpus);
                //double cputimeUsage = performance::CalcProcessor(previousNewTime, newTime, duration, 12);

                callback(procUsage);

                //log("cputimeUsage: ", cputimeUsage);
                previousCycleTime = processInfo.CycleTime;
                previousNewTime = newTime;

                {
                    //std::scoped_lock<std::mutex> lock;
                    //g_procUsage.push_back(procUsage);
                    //g_procUsage = procUsage;
                }

                Sleep(500);
            }
            
            // Process ended, so send CPU usage as zero
            callback(0);
        });

        return thread;
    }
}

std::wstring fanNoiseLevelToString(PO_FAN_NOISE_LEVEL level)
{
    switch (level)
    {
        case FanNoiseLevelOff:
            return L"FanNoiseLevelOff";
        case FanNoiseLevelLow:
            return L"FanNoiseLevelLow";
        case FanNoiseLevelMed:
            return L"FanNoiseLevelMed";
        case FanNoiseLevelHigh:
            return L"FanNoiseLevelHigh";
        case FanNoiseLevelMax:
            return L"FanNoiseLevelMax";
        default:
            throw std::runtime_error("Unknown level");
    }
}

//
// WNF_RM_DEVELOPER_QUIET_MODE_ACTIVE
//
//   Description : Supplies the status of developer quiet mode
//   Sequence    : 4
//   Type        : WnfWellKnownStateName
//   Scope       : WnfDataScopeSystem
//   SDDL        : D:(A;;1;;;WD)(A;;3;;;SY)(A;;1;;;AC)
//   Data size   : 4
//   Data format : <untyped>
//

EXTERN_C WNF_STATE_DECL WNF_STATE_NAME
    WNF_RM_DEVELOPER_QUIET_MODE_ACTIVE = {0xa3bc2075, 0x41c6033f};

namespace performance
{
    bool GetGameMode()
    {
        WNF_CHANGE_STAMP changeStamp;
        ULONG pid{};
        ULONG cb = sizeof(pid);
        THROW_IF_NTSTATUS_FAILED(NtQueryWnfStateData(&WNF_RM_GAME_MODE_ACTIVE, NULL, NULL, &changeStamp, &pid, &cb));
        return pid != 0;
    }

    void SetGameMode(bool enable)
    {
        int pid = (int)enable;
        THROW_IF_NTSTATUS_FAILED(RtlPublishWnfStateData(WNF_RM_GAME_MODE_ACTIVE, NULL, &pid, sizeof(pid), NULL));
    }
    
    bool GetDeveloperQuietMode()
    {
        WNF_CHANGE_STAMP changeStamp;
        ULONG value{};
        ULONG cb = sizeof(value);
        THROW_IF_NTSTATUS_FAILED(NtQueryWnfStateData(&WNF_RM_DEVELOPER_QUIET_MODE_ACTIVE, NULL, NULL, &changeStamp, &value, &cb));
        return value != 0;
    }

    void SetDeveloperQuietMode(bool enable)
    {
        ULONG value = enable ? 1 : 0;
        THROW_IF_NTSTATUS_FAILED(RtlPublishWnfStateData(WNF_RM_DEVELOPER_QUIET_MODE_ACTIVE, NULL, &value, sizeof(value), NULL));
    }

    unsigned long GetProcessPidByName(PCWSTR processName)
    {
        wil::unique_handle hSnapShot(CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL));
        THROW_LAST_ERROR_IF(hSnapShot.get() == INVALID_HANDLE_VALUE);
        PROCESSENTRY32 pEntry;
        pEntry.dwSize = sizeof(pEntry);
        BOOL hRes = Process32First(hSnapShot.get(), &pEntry);
        while (hRes)
        {
            if (wil::compare_string_ordinal(pEntry.szExeFile, processName, true) == 0)
            {
                return pEntry.th32ProcessID;
            }
            hRes = Process32Next(hSnapShot.get(), &pEntry);
        }
        return 0;
    }


/*
    bool GetUserPresence()
    {
        PO_USER_PRESENCE_HOST_STATE value = {};
        //THROW_IF_NTSTATUS_FAILED(NtPowerInformation(PowerInformationInternal, nullptr, 0, &value, sizeof(value)));
        THROW_IF_NTSTATUS_FAILED(NtPowerInformation(UserPresence, nullptr, 0, &value, sizeof(value)));

        return value.GlobalUserPresence == PowerUserPresent;
    }
*/

    using callback_t = void(*)(void*, ULONG, void*);
    void RegisterNotificationUserPresence(callback_t callback)
    {
        DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS params;
        params.Context = nullptr;
        params.Callback = (PDEVICE_NOTIFY_CALLBACK_ROUTINE) callback;
        auto cb = (HPOWERNOTIFY)nullptr;

        THROW_IF_WIN32_ERROR(PowerSettingRegisterNotification(&GUID_GLOBAL_USER_PRESENCE, DEVICE_NOTIFY_CALLBACK, &params, &cb));
    }

    /*
    NTSTATUS
    UmpoWnfGlobalUserPresenceHostStateCallback (
        _In_ WNF_STATE_NAME StateName,
        _In_ WNF_CHANGE_STAMP ChangeStamp,
        _In_opt_ PWNF_TYPE_ID TypeId,
        _In_opt_ PVOID CallbackContext,
        _In_opt_bytecount_(Length) const VOID *Buffer,
        _In_ ULONG Length
        )
        */
    void SetUserPresence(bool present)
    {
        USER_ACTIVITY_PRESENCE value = PowerUserInactive;
        if (present)
        {
            value = PowerUserPresent;
        }
        else
        {
            value = PowerUserInactive;
            //value = PowerUserNotPresent;
        }

        PO_USER_PRESENCE_HOST_STATE val;
        RtlZeroMemory(&val, sizeof(PO_USER_PRESENCE_HOST_STATE));
        val.Header.InternalType = PowerInternalHostGlobalUserPresenceStateUpdate;

        //val.GlobalUserPresence = *(PUSER_ACTIVITY_PRESENCE)Buffer;
        val.GlobalUserPresence = value;

        THROW_IF_NTSTATUS_FAILED(NtPowerInformation(PowerInformationInternal,
                                                &val,
                                                sizeof(val),
                                                NULL,
                                                0));
    }

    void SetEnergySaver(bool enable)
    {

        BOOLEAN value;
        if (enable)
        {
            value = TRUE;
        }
        else
        {
            value = FALSE;
        }

        PO_ENERGY_SAVER_HOST_STATE val;
        RtlZeroMemory(&val, sizeof(PO_USER_PRESENCE_HOST_STATE));
        val.Header.InternalType = PowerInternalHostEsStateUpdate;
        val.Engaged = value;

        //val.GlobalUserPresence = *(PUSER_ACTIVITY_PRESENCE)Buffer;
        //val.GlobalUserPresence = value;

        THROW_IF_NTSTATUS_FAILED(NtPowerInformation(PowerInformationInternal,
                                                &val,
                                                sizeof(val),
                                                NULL,
                                                0));
    }

    void SetSystemBatteryState()
    {
        //SystemBatteryState
    }

    bool IsBatterySaverEnabled()
    {
        RM_QUIET_MODE_DATA quietModeData {};

        return wil::wnf_query(WNF_RM_QUIET_MODE, &quietModeData) && quietModeData.BatterySaver;
    }

    /*
    bool Power::IsSystemInConnectedStandby()
    {
        SEB_EVENT_HEADER data {};

        return IsConnectedStandbySupported() && wil::wnf_query(WNF_SEB_SYSTEM_LPE, &data) && (data.Level == 1);
    }
    */

    std::tuple<bool, bool, float> GetBatteryInfo()
    {
        SYSTEM_BATTERY_STATE batteryState = {};
        THROW_IF_NTSTATUS_FAILED(NtPowerInformation(SystemBatteryState, nullptr, 0, &batteryState, sizeof(batteryState)));

        /*
            if (!batteryState.BatteryPresent)
            {
            THROW_NTSTATUS(STATUS_DEVICE_DOES_NOT_EXIST);
        }
        */

        if ((batteryState.RemainingCapacity == BATTERY_UNKNOWN_CAPACITY) ||
            (batteryState.MaxCapacity == BATTERY_UNKNOWN_CAPACITY) ||
            (batteryState.MaxCapacity == 0))
        {
            //THROW_NTSTATUS(STATUS_INVALID_DEVICE_STATE);
            batteryState.RemainingCapacity = 100;
            batteryState.MaxCapacity = 100;
        }

        float batteryPercent = (float) ((batteryState.RemainingCapacity * 100) / batteryState.MaxCapacity);

        if (batteryPercent > 100)
        {
            batteryPercent = 100;
        }

        return {batteryState.BatteryPresent, batteryState.AcOnLine, batteryPercent};
    }


}


namespace crmclient
{
    using unique_crm_registration_context_handle = wil::unique_any<CRM_REGISTRATION_CONTEXT_HANDLE, decltype(&CrmUnregister), CrmUnregister>;
    using unique_activity_handle = wil::unique_any<CRM_ACTIVITY_CONTEXT_HANDLE, decltype(&CrmActivityFree), CrmActivityFree>;
    using unique_rtl_wnf_notification_completion_wait = wil::unique_any<PWNF_USER_SUBSCRIPTION, decltype(&RtlUnsubscribeWnfNotificationWaitForCompletion), RtlUnsubscribeWnfNotificationWaitForCompletion>;
    using unique_window_closed_reasons = wil::unique_any<CRM_WINDOW_CLOSED_REASON*, decltype(&CrmActivityFreeWindowClosedReasons), CrmActivityFreeWindowClosedReasons>;

    struct unique_crm_window_closed_reason_subscription
    {
        unique_crm_window_closed_reason_subscription() {};

        // no copy
        unique_crm_window_closed_reason_subscription(const unique_crm_window_closed_reason_subscription&) = delete;
        unique_crm_window_closed_reason_subscription& operator=(const unique_crm_window_closed_reason_subscription& other) = delete;

        // movable
        unique_crm_window_closed_reason_subscription(unique_crm_window_closed_reason_subscription&& other)
        {
            m_activity = other.m_activity;
            other.m_activity = 0;
        }
        unique_crm_window_closed_reason_subscription& operator=(unique_crm_window_closed_reason_subscription&& other)
        {
            m_activity = other.m_activity;
            other.m_activity = 0;
            return *this;
        }

        unique_crm_window_closed_reason_subscription(CRM_ACTIVITY_CONTEXT_HANDLE activity, PCRM_ACTIVITY_WINDOW_CLOSED_CALLBACK callback) : m_activity(activity)
        {
            THROW_IF_NTSTATUS_FAILED(CrmActivityWindowClosedReasonSubscribe(activity, callback, NULL));
        }
        
        ~unique_crm_window_closed_reason_subscription()
        {
            if (m_activity)
            {
                CrmActivityWindowClosedReasonUnsubscribe(m_activity);
            }
        }
        CRM_ACTIVITY_CONTEXT_HANDLE m_activity{};
    };

    struct unique_crm_activity_start
    {
        unique_crm_activity_start() {};

        // no copy
        unique_crm_activity_start(const unique_crm_activity_start&) = delete;
        unique_crm_activity_start& operator=(const unique_crm_activity_start& other) = delete;

        // movable
        unique_crm_activity_start(unique_crm_activity_start&& other)
        {
            m_activity = other.m_activity;
            other.m_activity = 0;
        }
        unique_crm_activity_start& operator=(unique_crm_activity_start&& other)
        {
            m_activity = other.m_activity;
            other.m_activity = 0;
            return *this;
        }

        unique_crm_activity_start(CRM_ACTIVITY_CONTEXT_HANDLE activity, LPWSTR simpleTaskDescription, LPWSTR simpleSubTaskDescription, CRM_ACTIVITY_START_REASON reason) : m_activity(activity)
        {
            THROW_IF_NTSTATUS_FAILED(CrmActivityStart(activity, simpleTaskDescription, simpleSubTaskDescription, reason));
        }
        
        ~unique_crm_activity_start()
        {
            if (m_activity)
            {
                CrmActivityStop(m_activity);
            }
        }
        CRM_ACTIVITY_CONTEXT_HANDLE m_activity{};
    };

    typedef union _RM_QUIET_MODE_DATA2 {
        struct {
            ULONG BatterySaver : 1;
            ULONG GameMode : 1;
            ULONG ConstrainedMode : 1;
            ULONG PostShellReady : 1;

            ULONG PostResume : 1;
            ULONG PowerConstrained : 1;
            ULONG Thermal : 1;
            ULONG CsAndDc : 1;

            ULONG DeveloperQuietMode : 1;

            ULONG FutureReserved : 23;
        };

        ULONG AllFlags;
    } RM_QUIET_MODE_DATA2, *PRM_QUIET_MODE_DATA2;

    std::vector<std::wstring> QuietModeDataToReasons(RM_QUIET_MODE_DATA quietModeData1)
    {
        // todo fix
        RM_QUIET_MODE_DATA2 quietModeData;
        quietModeData.AllFlags = quietModeData1.AllFlags;

        std::vector<std::wstring> reasons;
        if (quietModeData.BatterySaver) {
            reasons.push_back(L"Battery saver");
        }

        if (quietModeData.GameMode) {
            reasons.push_back(L"Game mode");
        }

        if (quietModeData.ConstrainedMode) {
            reasons.push_back(L"Constrained mode");
        }

        if (quietModeData.PostShellReady) {
            reasons.push_back(L"Post shell ready");
        }

        if (quietModeData.PostResume) {
            reasons.push_back(L"Post resume");
        }

        if (quietModeData.PowerConstrained) {
            reasons.push_back(L"PowerConstrained");
        }

        if (quietModeData.Thermal) {
            reasons.push_back(L"Thermal");
        }

        if (quietModeData.CsAndDc) {
            reasons.push_back(L"CS and DC");
        }

        if (quietModeData.DeveloperQuietMode) {
            reasons.push_back(L"Developer quiet mode");
        }
        return reasons;
    }

    std::wstring NotificationTypeToString(CRM_NOTIFICATION_TYPE notificationType)
    {
        switch (notificationType)
        {
        case CrmNotificationTypeActivityWindowOpen:
            return L"Open";
        case CrmNotificationTypeActivityWindowClosed:
            return L"Closed";
        case CrmNotificationTypeActivityExpiration:
            return L"Expired";
        case CrmNotificationTypeActivityDeadlineExpiration:
            return L"Deadline expired";
        }
        THROW_HR(E_UNEXPECTED);
    }

    std::wstring WindowClosedReasonToString(CRM_WINDOW_CLOSED_REASON reason)
    {
        // todo fix
        auto const CrmWindowClosedReasonDeveloperQuietMode = 42;
        switch (reason)
        {
            case CrmWindowClosedReasonTest0:
                return L"Test reason 0";
            case CrmWindowClosedReasonTest1:
                return L"Test reason 1";
            case CrmWindowClosedReasonCompositeTest:
                return L"Composite test";
            case CrmWindowClosedReasonGlobalUserPresence:
                return L"User presence";
            case CrmWindowClosedReasonAcDc:
                return L"AC/DC state";
            case CrmWindowClosedReasonBatteryPercentage:
                return L"Battery percentage";
            case CrmWindowClosedReasonDisplayState:
                return L"Display state";
            case CrmWindowClosedReasonPowerSavings:
                return L"Battery saver";
            case CrmWindowClosedReasonConnectedStandby:
                return L"Connected standby";
            case CrmWindowClosedReasonGameMode:
                return L"Game mode";
            case CrmWindowClosedReasonSystemSuspend:
                return L"System suspended state";
            case CrmWindowClosedReasonTimeSinceBoot:
                return L"Time since boot";
            case CrmWindowClosedReasonTimeSinceLogon:
                return L"Time since logon";
            case CrmWindowClosedReasonInternet:
                return L"Internet availability";
            case CrmWindowClosedReasonFreeInternet:
                return L"Free internet availability";
            case CrmWindowClosedReasonIPAddress:
                return L"IP address availability";
            case CrmWindowClosedReasonConstrainedMode:
                return L"Constrained mode";
            case CrmWindowClosedReasonNonCellularConnected:
                return L"Non cellular connection";
            case CrmWindowClosedReasonNetwork:
                return L"Network";
            case CrmWindowClosedReasonPostShellReady:
                return L"Post shell ready";
            case CrmWindowClosedReasonPostResume:
                return L"Post resume";
            case CrmWindowClosedReasonPowerConstrained:
                return L"Power constrained";
            case CrmWindowClosedReasonThermal:
                return L"Thermal";
            case CrmWindowClosedReasonLockState:
                return L"Lock state";
            case CrmWindowClosedReasonCPUUsage:
                return L"CPU usage";
            case CrmWindowClosedReasonSystemDiskUsage:
                return L"System disk usage";
            case CrmWindowClosedReasonDelayedAc:
                return L"Delayed AC";
            case CrmWindowClosedReasonGPUUsage:
                return L"GPU usage";
            case CrmWindowClosedReasonConnectivityInStandby:
                return L"Connectivity in standby";
            case CrmWindowClosedReasonDisconnectedStandby:
                return L"Disconnected standby";
            case CrmWindowClosedReasonAudioPlayback:
                return L"Audio Playback";
            case CrmWindowClosedReasonDelayedCs:
                return L"Delayed CS";
            case CrmWindowClosedReasonSebSystemIdle:
                return L"SEB System Idle";
            case CrmWindowClosedReasonAcDcDisplay:
                return L"AC/DC Display";
            case CrmWindowClosedReasonAcDcBatteryPercentage:
                return L"AC/DC Battery Percentage";
            case CrmWindowClosedReasonDelayedInternetLoss:
                return L"Delayed Internet Loss";
            case CrmWindowClosedReasonSuspendAppBackgroundActivity:
                return L"Shell Suspend App Background Activity";
            case CrmWindowClosedReasonFanNoise:
                return L"Fan Noise";
            case CrmWindowClosedReasonFanNoiseRaw:
                return L"Fan Noise Raw";
            case CrmWindowClosedReasonDeveloperQuietMode:
                return L"Developer quiet mode";
        }
        return L"Unknown reason";
    }

    unique_rtl_wnf_notification_completion_wait
    RegisterQuietMode(PWNF_USER_CALLBACK callback)
    {
        unique_rtl_wnf_notification_completion_wait wnfSubscription;
        THROW_IF_NTSTATUS_FAILED(RtlSubscribeWnfStateChangeNotification(&wnfSubscription, WNF_RM_QUIET_MODE, 0, callback, NULL, NULL, 0, 0));
        return wnfSubscription;
    }

    unique_crm_registration_context_handle
    RegisterClient(ULONG clientId, PCWSTR simpleClientDescription = L"CrmClientTest")
    {
        auto ClientId = (CRM_CLIENT_ID)clientId;

        LPWSTR desc = (LPWSTR)simpleClientDescription;
        unique_crm_registration_context_handle registrationHandle;
        THROW_IF_NTSTATUS_FAILED(CrmRegister(&registrationHandle, ClientId, desc));
        return registrationHandle;
    }

    std::tuple<unique_activity_handle, unique_crm_window_closed_reason_subscription>
    CreateActivity(CRM_REGISTRATION_CONTEXT_HANDLE registration, ULONG activityId, PCRM_ACTIVITY_CALLBACK callback, PCRM_ACTIVITY_WINDOW_CLOSED_CALLBACK reasonCallback, void* context = nullptr, PCWSTR simpleActivityDescription = L"CrmClientActivity")
    {
        auto ActivityId = (CRM_ACTIVITY_ID)activityId;

        LPWSTR desc = (LPWSTR)simpleActivityDescription;
        unique_activity_handle activityHandle;
        THROW_IF_NTSTATUS_FAILED(CrmActivityAllocate(&activityHandle, registration, ActivityId, desc, NULL, callback, context));
        THROW_IF_NTSTATUS_FAILED(CrmActivityRequest(activityHandle.get()));

        auto windowCloseSubscription = unique_crm_window_closed_reason_subscription(activityHandle.get(), reasonCallback);
        return { std::move(activityHandle), std::move(windowCloseSubscription) };
    }
}
