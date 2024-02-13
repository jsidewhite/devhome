#include "pch.h"
#include "QuietBackgroundProcessesSessionManager.h"
#include "QuietBackgroundProcessesSessionManager.g.cpp"

namespace winrt::DevHome::QuietBackgroundProcesses::implementation
{
    bool acquired = false;
    winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession QuietBackgroundProcessesSessionManager::GetSession()
    {
        //auto x = winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession::GetSingleton();
        //acquired = true;
        //return x;
        return nullptr;
    }
    winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession QuietBackgroundProcessesSessionManager::TryGetSession()
    {
        if (acquired)
        {
            //static auto x = winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession::GetSingleton();
            //return winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession::GetSingleton();
        }
        //return nullptr;
        return winrt::DevHome::QuietBackgroundProcesses::QuietBackgroundProcessesSession::GetSingleton();
    }
}
