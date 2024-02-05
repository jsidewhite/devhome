#include "pch.h"

#include <windows.h>
#include <psapi.h>
#include <iostream>
#include <string>

double GetProcessCpuUsage(DWORD processId)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);

    if (hProcess == NULL)
    {
        std::cerr << "Failed to open process. Error code: " << GetLastError() << std::endl;
        return -1.0;
    }

    FILETIME createTime, exitTime, kernelTime, userTime;

    if (GetProcessTimes(hProcess, &createTime, &exitTime, &kernelTime, &userTime) == 0)
    {
        std::cerr << "Failed to get process times. Error code: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return -1.0;
    }

    ULARGE_INTEGER totalTime;
    totalTime.LowPart = userTime.dwLowDateTime;
    totalTime.HighPart = userTime.dwHighDateTime;

    totalTime.LowPart += kernelTime.dwLowDateTime;
    totalTime.HighPart += kernelTime.dwHighDateTime;

    double cpuUsage = (totalTime.QuadPart * 100.0) / (GetTickCount64() * 10000.0);

    CloseHandle(hProcess);

    return cpuUsage;
}

void MonitorCpuUsage()
{
    DWORD processes[1024];
    DWORD needed;
    if (EnumProcesses(processes, sizeof(processes), &needed) == 0)
    {
        std::cerr << "Failed to enumerate processes. Error code: " << GetLastError() << std::endl;
        return;
    }

    int numProcesses = needed / sizeof(DWORD);

    for (int i = 0; i < numProcesses; i++)
    {
        double cpuUsage = GetProcessCpuUsage(processes[i]);
        if (cpuUsage >= 0.0)
        {
            std::cout << "Process ID: " << processes[i] << " - CPU Usage: " << cpuUsage << "%" << std::endl;
        }
    }
}
