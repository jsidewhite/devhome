#include <pch.h>
#include <Windows.h>

class YourComClass : public IUnknown
{
private:
    ULONG m_refCount;

public:
    YourComClass() :
        m_refCount(1) {}

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
    {
        if (riid == IID_IUnknown)
        {
            *ppvObject = static_cast<IUnknown*>(this);
            AddRef();
            return S_OK;
        }
        *ppvObject = nullptr;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG)
    AddRef()
    {
        return InterlockedIncrement(&m_refCount);
    }

    STDMETHODIMP_(ULONG)
    Release()
    {
        ULONG refCount = InterlockedDecrement(&m_refCount);
        if (refCount == 0)
            delete this;
        return refCount;
    }

    // ISimpleInterface methods
    STDMETHODIMP DoSomething()
    {
        // Implementation of your COM class's method
        return S_OK;
    }
};