#include "pch.h"
#include "QuietWindow.h"
#include "QuietWindow.g.cpp"

namespace winrt::QuietBackgroundProcesses_ElevatedServer::implementation
{
    bool QuietWindow::IsActive()
    {
        return true;
    }
    int32_t QuietWindow::TimeLeftInSeconds()
    {
        return 9;
    }
}
