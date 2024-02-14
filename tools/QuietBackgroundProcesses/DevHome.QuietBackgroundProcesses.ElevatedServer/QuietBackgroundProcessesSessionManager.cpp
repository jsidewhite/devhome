#include "pch.h"

#include <wil/winrt.h>

#include "QuietBackgroundProcessesSessionManager.h"
#include "QuietBackgroundProcessesSessionManager.g.cpp"

namespace winrt::DevHome::QuietBackgroundProcesses::implementation
{
    bool acquired = false;

    winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession QuietBackgroundProcessesSessionManager::GetSession()
    {
        auto fac = winrt::get_activation_factory<winrt::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionStatics>(L"DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession");
        //auto inst = fac.ActivateInstance<winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession>();
        acquired = true;
        //return inst.GetSingleton();

        return fac.GetSingleton();
        /*
        //return fac->;
        return winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession::GetSingleton();
        */
    }

    winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession QuietBackgroundProcessesSessionManager::TryGetSession()
    {
        if (acquired)
        {
            //static auto x = winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession::GetSingleton();
            //auto x = winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession::GetSingleton();
            //acquired = true;
            //return x;
            auto fac = winrt::get_activation_factory<winrt::DevHome::QuietBackgroundProcesses::IQuietBackgroundProcessesSessionStatics>(L"DevHome.QuietBackgroundProcesses.QuietBackgroundProcessesSession");

            return fac.GetSingleton();
            //            return winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession::GetSingleton();
        }
        return nullptr;
        //return winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession::GetSingleton();
    }
    winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesThingy QuietBackgroundProcessesSessionManager::TryGetThingy()
    {
        return winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesThingy();
        //return winrt::make<winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesThingy>();
    }

    
    int32_t QuietBackgroundProcessesSessionManager::GetInt()
    {
        return 823;
    }
}
