#include "pch.h"

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

// wnf stuff
#include "power.h"
#include "wnfnamesp.h"

// power stuff
#include "ntpoapi_p.h"

// power saver (RM_QUIET_MODE_DATA)
#include "crmtypes.h"

// battery stuff
#include "poclass.h"

// process stuff
#include <tlhelp32.h>

// mine
#include <..\inc\include.h>
#include <..\inc\include_crm.h>
#include <..\inc\include_activitycoordinator.h>



//
// C++ -> JS Callbacks
//
using fn_loga_t = void (*)(const char *str);
using fn_logw_t = void (*)(const wchar_t *str);
static fn_loga_t g_electronLogA;
static fn_logw_t g_electronLogW;

extern "C" void __declspec(dllexport) RegisterFunctions(fn_loga_t electronLogA, fn_logw_t electronLogW)
{
    g_electronLogA = electronLogA;
    g_electronLogW = electronLogW;
}

extern "C" void __declspec(dllexport) ElectronLogA(const char *message)
{
    g_electronLogA(message);
}

extern "C" void __declspec(dllexport) ElectronLogW(const wchar_t *message)
{
    g_electronLogW(message);
}

//
// C++ -> JS Callbacks extended
//
/*
using fn_callback_quietmodebits_t = void (*)(unsigned long);
using fn_callback_activitywindowopen_t = void (*)(PCWSTR, bool);
using fn_callback_activitywindowclosedreasons_t = void (*)(PCWSTR, PCWSTR);

static fn_callback_quietmodebits_t g_callback_notifyquietmodebits;
static fn_callback_activitywindowopen_t g_callback_activitywindowopen;
static fn_callback_activitywindowclosedreasons_t g_callback_activitywindowclosedreasons;

extern "C" void __declspec(dllexport) RegisterCallbackFunctions(fn_callback_quietmodebits_t fn1, fn_callback_activitywindowopen_t fn2, fn_callback_activitywindowclosedreasons_t fn3)
{
    g_callback_notifyquietmodebits = fn1;
    g_callback_activitywindowopen = fn2;
    g_callback_activitywindowclosedreasons = fn3;
}

extern "C" void __declspec(dllexport) NotifyQuietModeBits(RM_QUIET_MODE_DATA bits)
{
    g_callback_notifyquietmodebits(bits.AllFlags);
}

extern "C" void __declspec(dllexport) NotifyActivityWindowOpen(PCWSTR identifier, bool open)
{
    g_callback_activitywindowopen(identifier, open);
}

extern "C" void __declspec(dllexport) NotifyActivityWindowClosedReasons(PCWSTR identifier, PCWSTR reasonsString)
{
    g_callback_activitywindowclosedreasons(identifier, reasonsString);
}
*/


//
// C++ -> JS Callbacks extended
//
namespace callbacks
{
    namespace quietmodebits
    {
        using type = void (*)(ULONG);
        static type fn;
    }

    namespace activitywindowopen
    {
        using type = void (*)(PCWSTR, bool);
        static type fn;
    }

    namespace activitywindowclosedreasons
    {
        using type = void (*)(PCWSTR, PCWSTR);
        static type fn;
    }

    namespace activitycoordinatorwindowchanged
    {
        using type = void (*)(PCWSTR, ULONG);
        static type fn;
    }
}

extern "C" void __declspec(dllexport) RegisterCallbackFunctions(
    callbacks::quietmodebits::type fn1,
    callbacks::activitywindowopen::type fn2,
    callbacks::activitywindowclosedreasons::type fn3,
    callbacks::activitycoordinatorwindowchanged::type fn4)
{
    callbacks::quietmodebits::fn = fn1;
    callbacks::activitywindowopen::fn = fn2;
    callbacks::activitywindowclosedreasons::fn = fn3;
    callbacks::activitycoordinatorwindowchanged::fn = fn4;
}

extern "C" void __declspec(dllexport) NotifyQuietModeBits(RM_QUIET_MODE_DATA bits)
{
    callbacks::quietmodebits::fn(bits.AllFlags);
}

extern "C" void __declspec(dllexport) NotifyActivityWindowOpen(PCWSTR identifier, bool open)
{
    callbacks::activitywindowopen::fn(identifier, open);
}

extern "C" void __declspec(dllexport) NotifyActivityWindowClosedReasons(PCWSTR identifier, PCWSTR reasonsString)
{
    callbacks::activitywindowclosedreasons::fn(identifier, reasonsString);
}

extern "C" void __declspec(dllexport) NotifyActivityCoordinatorWindowChanged(PCWSTR identifier, ULONG state)
{
    callbacks::activitycoordinatorwindowchanged::fn(identifier, state);
}

//
// Util
//
namespace details2
{
    template <typename Func, typename... Ts>
    void for_each_parameter_pack(Func f, Ts&&... args)
    {
        int ignored[] = { (f(std::forward<Ts&>(args)), 0)...};
        (void) ignored;
    }

    template <typename... Ts>
    std::wstring format_string(Ts&&... args)
    {
        std::wstringstream ss;
        for_each_parameter_pack([&ss](auto x){ ss << x; }, std::forward<Ts&>(args)...);
        return ss.str();
    }
}

template <typename... Ts>
std::wstring format(Ts&&... args)
{
    return details2::format_string(std::forward<Ts&>(args)...);
}

std::wstring join_string(std::vector<std::wstring> const & strings, std::wstring_view joinstring)
{
    std::wstring result;
    auto i = 0;
    for (const auto & string : strings)
    {
        if (i++ == 0)
        {
            result += string;
        }
        else
        {
            result += joinstring.data() + string;
        }
    }
    return result;
}

template <typename... Ts>
void jsprint(Ts&&... args)
{
    auto message = details2::format_string(std::forward<Ts&>(args)...);
    g_electronLogW(message.c_str());
}





//
// Exports
//
std::atomic<bool> g_cpuLoadRunning{};
extern "C" void __declspec(dllexport) PerformanceState_SimulateCpuLoadStart(int cpus)
{
    g_cpuLoadRunning = true;
    for (int i = 0; i < cpus; i++)
    {
        auto thread = std::thread([]()
        {
            while (g_cpuLoadRunning)
            {
                clock_t wakeup = clock() + 50;
                while(clock() < wakeup) {}
                //Sleep(50);
            }
        });
        thread.detach();
    }
}

extern "C" void __declspec(dllexport) PerformanceState_SimulateCpuLoadStop()
{
    g_cpuLoadRunning = false;
}

std::vector<std::pair<std::promise<void>, std::thread>> g_threads;
std::mutex g_lock;
extern "C" ULONG __declspec(dllexport) PerformanceState_SimulateCpuLoadAdd()
{
    std::promise<void> p;
    auto fut = p.get_future();
    //auto thread = std::thread([p = std::move(p)]()
    //auto thread = std::thread([p = std::move(p)]()
    auto thread = std::thread([fut = std::move(fut)]()
    {
        std::future_status status;
        do
        {
            switch (status = fut.wait_for(std::chrono::milliseconds(10)); status)
            {
                case std::future_status::deferred: break;
                case std::future_status::timeout: break;
                case std::future_status::ready: break;
            }

            clock_t wakeup = clock() + 50;
            while (clock() < wakeup) {}
        }
        while (status != std::future_status::ready);
    });
    //thread.detach();

    auto lock = std::scoped_lock<std::mutex>(g_lock);
    g_threads.emplace_back(std::move(p), std::move(thread));
    return static_cast<ULONG>(g_threads.size());
}

extern "C" ULONG __declspec(dllexport) PerformanceState_SimulateCpuLoadRemove()
{
    auto lock = std::scoped_lock<std::mutex>(g_lock);
    if (g_threads.size() > 0)
    {
        auto pair = std::move(g_threads.back());
        g_threads.pop_back();
        pair.first.set_value();
        pair.second.join();
    }
    return static_cast<ULONG>(g_threads.size());
}

extern "C" ULONG __declspec(dllexport) PerformanceState_GetProcessPidByName(PCWSTR processName)
{
    return performance::GetProcessPidByName(processName);
}

extern "C" ULONG __declspec(dllexport) PerformanceState_GetGameMode()
{
    return performance::GetGameMode() ? 1 : 0;
}

extern "C" bool __declspec(dllexport) PerformanceState_GetDeveloperQuietMode()
{
    return performance::GetDeveloperQuietMode();
}

//std::atomic<double> g_cpuUsage;
//std::map<ULONG, double> g_cpuUsages;
//std::map<ULONG, std::tuple<std::thread, double>> g_cpuUsages;
std::map<ULONG, std::atomic<double>> g_cpuUsages;
extern "C" HRESULT __declspec(dllexport) PerformanceState_StartForProcess(ULONG pid)
{
    static std::mutex s_mutex;
    auto lock = std::scoped_lock<std::mutex>(s_mutex);
    auto thread = performance::RunPerformanceMonitor(pid, [pid](double cpuUsage)
    {
        //g_cpuUsage = cpuUsage;
        g_cpuUsages[pid] = cpuUsage;
    });
    thread.detach();
    return S_OK;
}

extern "C" ULONG __declspec(dllexport) PerformanceState_GetCpuUsageForProcess(ULONG pid)
{
    //return static_cast<ULONG>(g_cpuUsage);
    return static_cast<ULONG>(g_cpuUsages[pid]);
}

extern "C" ULONG __declspec(dllexport) PerformanceState_GetBatteryInfo_BatteryPresent()
{
    return std::get<0>(performance::GetBatteryInfo());
}
extern "C" ULONG __declspec(dllexport) PerformanceState_GetBatteryInfo_AcOnLine()
{
    return std::get<1>(performance::GetBatteryInfo());
}
extern "C" ULONG __declspec(dllexport) PerformanceState_GetBatteryInfo_BatteryPercent()
{
    return (ULONG)std::get<2>(performance::GetBatteryInfo());
}
extern "C" bool __declspec(dllexport) PerformanceState_IsBatterySaverEnabled()
{
    return performance::IsBatterySaverEnabled();
}

std::once_flag g_flag1;
//std::once_flag g_flag2;
extern "C" ULONG __declspec(dllexport) PerformanceState_GetUserPresence()
{
    static std::atomic<bool> s_userPresence{};
    //static std::promise<void> g_promise;

    std::call_once(g_flag1, [](){

        //g_promise.set_value();
        performance::RegisterNotificationUserPresence([](void*, ULONG type, void* setting)
        {
            THROW_HR_IF(E_UNEXPECTED, type != PBT_POWERSETTINGCHANGE);
            auto powerSetting = (PPOWERBROADCAST_SETTING) setting;
            THROW_HR_IF(E_UNEXPECTED, powerSetting->DataLength != sizeof(UserPresence));
            
            USER_ACTIVITY_PRESENCE userPresence{};
            memcpy(&userPresence, &powerSetting->Data, powerSetting->DataLength);

            s_userPresence = (userPresence == PowerUserPresent);

            /*
            std::call_once(g_flag2, [](){
                g_promise.set_value();
            });
            */
        });
        Sleep(100);

        //g_promise.get_future().get();
    });

    return s_userPresence ? 1 : 0;
}

extern "C" void __declspec(dllexport) PerformanceState_SetUserPresence(bool present)
{
    performance::SetUserPresence(present);
}


//
//
//
// CRM Client
//
//
//

// global state
static std::mutex g_lock2;
std::map<ULONG, crmclient::unique_crm_registration_context_handle> g_clientRegistrations;
std::map<std::wstring, crmclient::unique_activity_handle> g_activityHandles;
std::map<CRM_ACTIVITY_CONTEXT_HANDLE,
    std::tuple<std::wstring, crmclient::unique_crm_window_closed_reason_subscription>> g_activityInformations;
std::map<CRM_ACTIVITY_CONTEXT_HANDLE, crmclient::unique_crm_activity_start> g_runningActivities;

//crmclient::unique_crm_registration_context_handle g_registrationHandle;
//crmclient::unique_activity_handle g_activityHandle;
//crmclient::unique_crm_window_closed_reason_subscription g_windowCloseSubscription;
crmclient::unique_rtl_wnf_notification_completion_wait g_wnfSubscription;
//std::optional<crmclient::unique_crm_activity_start> g_runningActivity;


namespace callbacks
{
    NTSTATUS CrmQuietModeWNFCallback (
        _In_ WNF_STATE_NAME StateName,
        _In_ WNF_CHANGE_STAMP ChangeStamp,
        _In_opt_ PWNF_TYPE_ID TypeId,
        _In_opt_ PVOID CallbackContext,
        _In_reads_bytes_opt_(Length) const VOID* Buffer,
        _In_ ULONG Length
        ) noexcept try

    {
        UNREFERENCED_PARAMETER(StateName);
        UNREFERENCED_PARAMETER(ChangeStamp);
        UNREFERENCED_PARAMETER(TypeId);
        UNREFERENCED_PARAMETER(CallbackContext);

        THROW_HR_IF(E_FAIL, Length < sizeof(RM_QUIET_MODE_DATA));

        auto QuietModeData = *((RM_QUIET_MODE_DATA*)Buffer);

        //g_electronLogA("Quiet mode is %s", (QuietModeData.AllFlags != 0) ? "engaged.\n" : "disengaged.\n");
        //std::string str = std::format("Quiet mode is %s", (QuietModeData.AllFlags != 0) ? "engaged.\n" : "disengaged.\n");
        //std::wstring str = std::wstring("Quiet mode is ") + ((QuietModeData.AllFlags != 0) ? "engaged." : "disengaged.");
        //jsprint(str.c_str());

        auto str = std::wstring(L"");

        if (QuietModeData.AllFlags != 0)
        {
            auto reasons = join_string(crmclient::QuietModeDataToReasons(QuietModeData), L", ");
            str += L"(" + reasons + L")";
            //jsprint(L"Quiet mode reasons: ", reasons);
        }

        std::wstring value = QuietModeData.AllFlags != 0 ? L"Engaged" : L"Disengaged";
        
        jsprint(L"Quiet Mode (CRM): ", value, L" ", str);

        NotifyQuietModeBits(QuietModeData);

        return STATUS_SUCCESS;
    }
    CATCH_RETURN()

    void CrmCallback (
        _In_ CRM_REGISTRATION_CONTEXT_HANDLE RegistrationHandle,
        _In_ CRM_ACTIVITY_CONTEXT_HANDLE ActivityHandle,
        _In_ CRM_NOTIFICATION_TYPE NotificationType,
        _In_opt_ PVOID NotificationData,
        _In_ PVOID CallbackContext
        ) noexcept try
    {
        UNREFERENCED_PARAMETER(RegistrationHandle);
        UNREFERENCED_PARAMETER(ActivityHandle);
        UNREFERENCED_PARAMETER(NotificationData);
        UNREFERENCED_PARAMETER(CallbackContext);

        std::wstring identifier;
        {
            std::scoped_lock<std::mutex> lock(g_lock2);
            identifier = std::get<0>(g_activityInformations[ActivityHandle]);
        }
        NotifyActivityWindowOpen(identifier.c_str(), NotificationType == CrmNotificationTypeActivityWindowOpen);

        auto msg = crmclient::NotificationTypeToString(NotificationType);
        jsprint(identifier, L" - Activity Window: ", msg);
    }
    CATCH_LOG()

    void CrmWindowClosedReasonCallback (
        _In_ CRM_REGISTRATION_CONTEXT_HANDLE RegistrationHandle,
        _In_ CRM_ACTIVITY_CONTEXT_HANDLE ActivityHandle,
        _In_ PVOID CallbackContext
        ) noexcept try
    {
        UNREFERENCED_PARAMETER(RegistrationHandle);
        UNREFERENCED_PARAMETER(CallbackContext);
        
        ULONG ReasonsCount;
        crmclient::unique_window_closed_reasons Reasons;
        THROW_IF_NTSTATUS_FAILED(CrmActivityQueryWindowClosedReasons(ActivityHandle, &Reasons, &ReasonsCount));

        std::vector<std::wstring> reasons;
        for (ULONG ReasonIndex = 0; ReasonIndex < ReasonsCount; ReasonIndex += 1)
        {
            reasons.push_back(crmclient::WindowClosedReasonToString(Reasons.get()[ReasonIndex]));
        }

        std::wstring reasonsStr;
        if (reasons.size() > 0)
        {
            reasonsStr = std::wstring() + join_string(reasons, L", ");
        }

        std::wstring identifier;
        {
            std::scoped_lock<std::mutex> lock(g_lock2);
            identifier = std::get<0>(g_activityInformations[ActivityHandle]);
        }
        NotifyActivityWindowClosedReasons(identifier.c_str(), reasonsStr.c_str());
        jsprint(identifier, L" - Closed window reasons: ", reasonsStr);
    }
    CATCH_LOG()
}

/*
namespace internal
{
    crmclient::unique_rtl_wnf_notification_completion_wait CrmClient_RegisterQuietMode()
    {
        crmclient::unique_rtl_wnf_notification_completion_wait wnfSubscription;
        THROW_IF_NTSTATUS_FAILED(RtlSubscribeWnfStateChangeNotification(&wnfSubscription, WNF_RM_QUIET_MODE, 0, callbacks::CrmQuietModeWNFCallback, NULL, NULL, 0, 0));
        return wnfSubscription;
    }

    crmclient::unique_crm_registration_context_handle CrmClient_RegisterClient(ULONG clientId)
    {
        auto ClientId = (CRM_CLIENT_ID)clientId;

        crmclient::unique_crm_registration_context_handle registrationHandle;
        THROW_IF_NTSTATUS_FAILED(CrmRegister(&registrationHandle, ClientId, L"CrmClientTest"));
        return registrationHandle;
    }

    crmclient::unique_crm_registration_context_handle CrmClient_RegisterClient(ULONG clientId)
    {
        auto ClientId = (CRM_CLIENT_ID)clientId;

        crmclient::unique_crm_registration_context_handle registrationHandle;
        THROW_IF_NTSTATUS_FAILED(CrmRegister(&registrationHandle, ClientId, L"CrmClientTest"));
        return registrationHandle;
    }

    std::tuple<crmclient::unique_activity_handle, crmclient::unique_crm_window_closed_reason_subscription>
    CrmClient_CreateActivity(const crmclient::unique_crm_registration_context_handle& registration, ULONG activityId)
    {
        auto ActivityId = (CRM_ACTIVITY_ID)activityId;

        crmclient::unique_activity_handle activityHandle;
        THROW_IF_NTSTATUS_FAILED(CrmActivityAllocate(&activityHandle, registration.get(), ActivityId, L"CrmClientActivity", NULL, callbacks::CrmCallback, NULL));
        THROW_IF_NTSTATUS_FAILED(CrmActivityRequest(activityHandle.get()));

        auto windowCloseSubscription = crmclient::unique_crm_window_closed_reason_subscription(activityHandle.get(), callbacks::CrmWindowClosedReasonCallback);
        return { std::move(activityHandle), std::move(windowCloseSubscription) };
    }

    crmclient::unique_crm_activity_start CrmClient_Start(crmclient::unique_activity_handle const & activity)
    {
        return crmclient::unique_crm_activity_start(activity.get(), L"CrmClient", L"Start", CrmActivityStartReasonUserRequest);
    }

    void CrmClient_Stop(crmclient::unique_activity_handle& runningActivity)
    {
        runningActivity.reset();
    }
}
*/

// helpers
namespace
{
    CRM_REGISTRATION_CONTEXT_HANDLE get_or_create_crmclient_registration(ULONG clientId)
    {
        std::scoped_lock<std::mutex> lock(g_lock2);
        {
            auto it = g_clientRegistrations.find(clientId);
            if (it == g_clientRegistrations.end())
            {
                g_clientRegistrations.emplace(clientId, crmclient::RegisterClient(clientId));
                //g_clientRegistrations.insert(clientId, crmclient::RegisterClient(clientId));
            }
        }
        return g_clientRegistrations[clientId].get();
    }
}

// exports
extern "C" HRESULT __declspec(dllexport) CrmClient_RegisterQuietMode() noexcept try
{
    //g_wnfSubscription = internal::CrmClient_RegisterQuietMode();
    g_wnfSubscription = crmclient::RegisterQuietMode(callbacks::CrmQuietModeWNFCallback);
    return S_OK;
}
CATCH_RETURN()

extern "C" HRESULT __declspec(dllexport) CrmClient_Register(PCWSTR identifier, ULONG clientId, ULONG activityId) noexcept try
{
    //auto [a,b,c,d] = CrmClient_Register(4, 20);
    //auto [a,b,c,d] = CrmClient_Register(4, 1006);
    //auto [a,b,c,d] = CrmClient_Register(2, 17);
    //auto [a,b,c,d] = CrmClient_Register(4, 1004);
    //auto [a,b,c] = internal::CrmClient_Register(7, 101);
    //if (g_clientRegistrations)
    auto registration = get_or_create_crmclient_registration(clientId);

    //std::wstring(L"TestActivity_")
    {
        std::scoped_lock<std::mutex> lock(g_lock2);
        auto [activity, windowClosedReasonSubscription] = crmclient::CreateActivity(registration, activityId, callbacks::CrmCallback, callbacks::CrmWindowClosedReasonCallback, nullptr, identifier);
        g_activityInformations.emplace(activity.get(), std::make_tuple(std::wstring(identifier), std::move(windowClosedReasonSubscription)));
        g_activityHandles.emplace(identifier, std::move(activity));
    }

    //g_registrationHandle = std::move(a);
    //g_activityHandle = std::move(a);
    //g_windowCloseSubscription = std::move(b);
    return S_OK;
}
CATCH_RETURN()

extern "C" HRESULT __declspec(dllexport) CrmClient_Start(PCWSTR identifier) noexcept try
{
    //std::wstring id = identifier;

    std::scoped_lock<std::mutex> lock(g_lock2);
    auto activityHandle = g_activityHandles[identifier].get();
    //g_runningActivity = crmclient::unique_crm_activity_start(g_activityHandle.get(), L"CrmClientSi", L"Start", CrmActivityStartReasonUserRequest);
    //g_runningActivity = crmclient::unique_crm_activity_start(g_activityHandle.get(), L"CrmClientSi", L"Start", CrmActivityStartReasonNotification);
    g_runningActivities[activityHandle] = crmclient::unique_crm_activity_start(activityHandle, L"CrmClientSi", L"Start", CrmActivityStartReasonNotification);
    
    // HEREHEREHERE
    //NotifyActivityWindowOpen(identifier, true);
    return S_OK;
}
CATCH_RETURN()

extern "C" HRESULT __declspec(dllexport) CrmClient_Stop(PCWSTR identifier) noexcept try
{
    std::scoped_lock<std::mutex> lock(g_lock2);
    auto activityHandle = g_activityHandles[identifier].get();
    g_runningActivities[activityHandle] = crmclient::unique_crm_activity_start();
    return S_OK;
}
CATCH_RETURN()




//
//
//
// Activity Coordinator
//
//
//
struct ac_entry
{
    std::wstring identifier;
    activitycoordinator::unique_subscription subscription;
};

// global state
static std::mutex g_lockAc;
static std::atomic<size_t> g_acId = 100;
//std::vector<std::wstring> g_acIdentifiers;
std::map<std::wstring, ac_entry> g_acSubscriptions;
std::map<size_t, std::wstring> g_acInformations;
//std::map<CRM_ACTIVITY_CONTEXT_HANDLE, crmclient::unique_crm_activity_start> g_runningActivities;


// helpers
namespace
{
    void pause_resume_callback(_In_ ACTIVITY_COORDINATOR_NOTIFICATION notificationType, _In_ void* callbackContext)
    {
        //worker_context* workerContext = reinterpret_cast<worker_context*>(callbackContext);

        auto handle = reinterpret_cast<size_t>(callbackContext);

        std::wstring identifier;
        {
            std::scoped_lock<std::mutex> lock(g_lockAc);
            identifier = g_acInformations[handle];
        }
        
        switch (notificationType)
        {
        case ACTIVITY_COORDINATOR_NOTIFICATION_RUN:
            break;

        case ACTIVITY_COORDINATOR_NOTIFICATION_STOP:
            break;

        default:
            FAIL_FAST();
        }

        NotifyActivityCoordinatorWindowChanged(identifier.c_str(), static_cast<ULONG>(notificationType));
    }
}

// exports
extern "C" HRESULT __declspec(dllexport) AcClient_Register(PCWSTR identifier, ULONG policyId) noexcept try
{
    THROW_HR_IF(E_NOTIMPL, !IsCreateActivityCoordinatorPolicyPresent());
    
    // make policy    
    activitycoordinator::unique_policy policy;
    if (policyId == 0)
    {
        THROW_IF_FAILED(CreateActivityCoordinatorPolicy(ACTIVITY_COORDINATOR_POLICY_TEMPLATE_GOOD, &policy));
        //THROW_IF_FAILED(SetActivityCoordinatorPolicyResourceCondition(policy.get(), ACTIVITY_COORDINATOR_RESOURCE_GPU, ACTIVITY_COORDINATOR_CONDITION_GOOD));
    }
    else if (policyId == 1)
    {
        THROW_IF_FAILED(CreateActivityCoordinatorPolicy(ACTIVITY_COORDINATOR_POLICY_TEMPLATE_MEDIUM, &policy));
    }
    else
    {
        THROW_HR(E_INVALIDARG);
    }

    // create activity
    std::scoped_lock<std::mutex> lock(g_lockAc);
    auto handle = g_acId++;
    activitycoordinator::unique_subscription subscription;
    THROW_IF_FAILED(SubscribeActivityCoordinatorPolicy(policy.get(), pause_resume_callback, reinterpret_cast<void*>(handle), &subscription));
    
    // ac_entry
    auto entry = ac_entry{ identifier, std::move(subscription) };

    // track in global
    g_acInformations.emplace(handle, identifier);
    g_acSubscriptions.emplace(identifier, std::move(entry));

    return S_OK;
}
CATCH_RETURN()
