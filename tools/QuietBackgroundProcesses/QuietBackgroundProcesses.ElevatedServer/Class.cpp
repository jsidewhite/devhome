#include "pch.h"
#include "Class.h"
#include "Class.g.cpp"

namespace winrt::QuietBackgroundProcesses_ElevatedServer::implementation
{
    int32_t Class::MyProperty()
    {
        return static_cast<int32_t>(GetCurrentProcessId());
    }

    void Class::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}
