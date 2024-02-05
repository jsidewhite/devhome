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


namespace activitycoordinator
{
    using unique_policy = wil::unique_any<ACTIVITY_COORDINATOR_POLICY, decltype(&DestroyActivityCoordinatorPolicy), DestroyActivityCoordinatorPolicy>;
    using unique_subscription = wil::unique_any<ACTIVITY_COORDINATOR_SUBSCRIPTION, decltype(&UnsubscribeActivityCoordinatorPolicy), UnsubscribeActivityCoordinatorPolicy>;

    struct waitable_signal
    {
        std::mutex mutex;
        std::condition_variable conditionVariable;
        bool signalValue{};

        void wait()
        {
            std::unique_lock<std::mutex> lock(mutex);
            conditionVariable.wait(lock, [&]
            {
                return signalValue;
            });

            // unset signal
            signalValue = false;
        }
        
        template <typename Rep, typename Period>
        bool wait_for(const std::chrono::duration<Rep, Period>& duration)
        {
            auto lock = std::unique_lock<std::mutex>(mutex);
            bool result = conditionVariable.wait_for(lock, duration, [&]
            {
                return signalValue;
            });

            // unset signal
            signalValue = false;

            return result;
        }

        void trigger()
        {
            {
                auto lock = std::scoped_lock<std::mutex>(mutex);
                signalValue = true;
            }
            conditionVariable.notify_all();
        }
    };

    struct worker_context
    {
    private:
        waitable_signal unpauseSignal;
        waitable_signal completionSignal;
        std::atomic<bool> shouldPause{};
        std::thread workThread;
        std::function<void(worker_context*)> workFunction;

    public:
        worker_context(std::function<void(worker_context*)> workMethod) : workFunction(std::move(workMethod)) {}

        ~worker_context()
        {
            if (workThread.joinable())
            {
                workThread.join();
            }
        }

        void unpause()
        {
            static std::mutex mutex;
            auto lock = std::scoped_lock<std::mutex>(mutex);
            
            if (!workThread.joinable())
            {
                workThread = std::thread([workerContext = this]()
                {
                    workerContext->workFunction(workerContext);
                });
            }

            unpauseSignal.trigger();
        }

        void pause()
        {
            shouldPause = true;
        }

        bool pausing()
        {
            return shouldPause;
        }

        void wait_for_unpause()
        {
            unpauseSignal.wait();
            shouldPause = false; // reset the pause signal
        }
        
        void notify_complete()
        {
            completionSignal.trigger();
        }

        void wait_for_complete()
        {
            return completionSignal.wait();
        }

        template <typename Rep, typename Period>
        bool wait_for_complete_for(const std::chrono::duration<Rep, Period>& duration)
        {
            return completionSignal.wait_for(duration);
        }
    };

/*
    void MyActualWorker(worker_context* workerContext)
    {
        log("[Worker: ENTERING]");

        size_t workLeft = 4;
        
        // Keep processing work until being told to stop or all work has been completed.
        while (true)
        {
            log("[Worker: WORK LEFT = ", workLeft, "]");

            if (workLeft <= 0)
            {
                break;
            }

            if (workerContext->pausing())
            {
                log("[Worker: PAUSING]");
                workerContext->wait_for_unpause();
            }

            Sleep(1000); // The actual work is to sleep :p

            workLeft--;
        }
        
        log("[Worker: DONE WORK]");
        workerContext->notify_complete();
    }
*/

    void worker_context_pause_resume_callback(_In_ ACTIVITY_COORDINATOR_NOTIFICATION notificationType, _In_ void* callbackContext)
    {
        worker_context* workerContext = reinterpret_cast<worker_context*>(callbackContext);
        
        switch (notificationType)
        {
        case ACTIVITY_COORDINATOR_NOTIFICATION_RUN:
            workerContext->unpause();
            break;

        case ACTIVITY_COORDINATOR_NOTIFICATION_STOP:
            workerContext->pause();
            break;

        default:
            FAIL_FAST();
        }
    }

}

/*
void DeferredWorkEventCallback(_In_ ACTIVITY_COORDINATOR_NOTIFICATION notificationType, _In_ void* callbackContext)
{
    worker_context* workerContext = reinterpret_cast<worker_context*>(callbackContext);
    
    switch (notificationType)
    {
    case ACTIVITY_COORDINATOR_NOTIFICATION_RUN:
        log("[Callback: RUN]");
        workerContext->unpause();
        break;

    case ACTIVITY_COORDINATOR_NOTIFICATION_STOP:
        log("[Callback: PAUSE]");
        workerContext->pause();
        break;

    default:
        FAIL_FAST();
    }
}

int __cdecl wmain(int argc, wchar_t** argv) try
{
    auto args = parse_args(argc, argv);
    auto argswitches = parse_arg_switches(args);
    auto argstrings = parse_arg_strings(args);
    auto argmap = parse_arg_strings_as_map(args);

    if (argswitches.find(L"-break") != argswitches.end())
    {
        std::wcout << L"doing: __debugbreak()" << std::endl;
        __debugbreak();
    }
    
    auto timeout1 = try_get_named_arg_as<int>(argmap, L"-timeout1").value_or(5);
    std::wcout << "timeout1: " << timeout1 << std::endl;

    auto timeout2 = try_get_named_arg_as<int>(argmap, L"-timeout2").value_or(5);
    std::wcout << "timeout2: " << timeout2 << std::endl;

    auto timeout3 = try_get_named_arg_as<int>(argmap, L"-timeout3").value_or(5);
    std::wcout << "timeout3: " << timeout3 << std::endl;

    std::wcout << std::endl;

    auto workerContext = worker_context(MyActualWorker);

    {
        auto seconds = timeout1;
        log("Subscribing with GOOD policies... [waiting for ", seconds, " seconds]");
        
        unique_subscription subscription;
        {
            unique_policy policy;
            THROW_IF_FAILED(CreateActivityCoordinatorPolicy(ACTIVITY_COORDINATOR_POLICY_TEMPLATE_GOOD, &policy));
            THROW_IF_FAILED(SetActivityCoordinatorPolicyResourceCondition(policy.get(), ACTIVITY_COORDINATOR_RESOURCE_GPU, ACTIVITY_COORDINATOR_CONDITION_GOOD));
            THROW_IF_FAILED(SubscribeActivityCoordinatorPolicy(policy.get(), DeferredWorkEventCallback, &workerContext, &subscription));
        }

        if (workerContext.wait_for_complete_for(std::chrono::seconds(seconds)))
        {
            log("Done with GOOD policies!");
            return 0;
        }
    }

    log("Continuing...");
    log("");

    {
        auto seconds = timeout2;
        log("Subscribing with MEDIUM policies... [waiting for ", seconds, " seconds]");
        
        unique_subscription subscription;
        {
            unique_policy policy;
            THROW_IF_FAILED(CreateActivityCoordinatorPolicy(ACTIVITY_COORDINATOR_POLICY_TEMPLATE_MEDIUM, &policy));
            THROW_IF_FAILED(SetActivityCoordinatorPolicyResourceCondition(policy.get(), ACTIVITY_COORDINATOR_RESOURCE_GPU, ACTIVITY_COORDINATOR_CONDITION_MEDIUM));
            THROW_IF_FAILED(SubscribeActivityCoordinatorPolicy(policy.get(), DeferredWorkEventCallback, &workerContext, &subscription));
        }

        if (workerContext.wait_for_complete_for(std::chrono::seconds(seconds)))
        {
            log("Done with MEDIUM policies!");
            return 0;
        }
    }

    log("Continuing...");
    log("");

    {
        auto seconds = timeout3;
        log("Subscribing with POOR policies... [waiting for ", seconds, " seconds]");
        
        unique_subscription subscription;
        {
            unique_policy policy;
            THROW_IF_FAILED(CreateActivityCoordinatorPolicy(ACTIVITY_COORDINATOR_POLICY_TEMPLATE_MEDIUM, &policy));
            THROW_IF_FAILED(SubscribeActivityCoordinatorPolicy(policy.get(), DeferredWorkEventCallback, &workerContext, &subscription));
        }

        if (workerContext.wait_for_complete_for(std::chrono::seconds(seconds)))
        {
            log("Done with POOR policies!");
            return 0;
        }
    }

    log("Continuing...");
    log("");

    log("Starting work manually...");
    workerContext.unpause();
    workerContext.wait_for_complete();

    log("Done with MANUAL policy.");

    return 0;
}
catch(std::exception const & e)
{
    auto hr = wil::ResultFromCaughtException();
    std::wcerr << L"0x" << std::hex << hr << std::endl;

    // Also spit out the error
    std::wcerr << e.what() << std::endl;
    return hr;
}
catch(...)
{
    auto hr = wil::ResultFromCaughtException();
    std::wcerr << L"0x" << std::hex << hr << std::endl;
    return hr;
}
*/
