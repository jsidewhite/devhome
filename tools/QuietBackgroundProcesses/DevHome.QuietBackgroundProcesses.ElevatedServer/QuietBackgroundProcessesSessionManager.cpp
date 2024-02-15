#include "pch.h"

#include <atomic>

#include <wil/winrt.h>

#include "QuietBackgroundProcessesSessionManager.h"
#include "QuietBackgroundProcessesSessionManager.g.cpp"

namespace winrt::DevHome::QuietBackgroundProcesses::implementation
{
    std::atomic<bool> acquired = false;

    winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession QuietBackgroundProcessesSessionManager::GetSession()
    {
        auto factory = winrt::get_activation_factory<winrt::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionStatics>(L"DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession");
        acquired = true;
        return factory.GetSingleton();
    }

    winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession QuietBackgroundProcessesSessionManager::TryGetSession()
    {
        if (acquired)
        {
            auto factory = winrt::get_activation_factory<winrt::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionStatics>(L"DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession");
            return factory.GetSingleton();
        }
        return nullptr;
    }
}
