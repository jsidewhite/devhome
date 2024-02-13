

// #include <Windows.h>
#include "simpleclass.h"

class SimpleClassFactory2 : public IClassFactory
{
private:
    ULONG m_refCount;

public:
    SimpleClassFactory2() :
        m_refCount(1) {}

    // IUnknown methods
    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject)
    {
        if (riid == IID_IUnknown || riid == IID_IClassFactory)
        {
            *ppvObject = static_cast<IClassFactory*>(this);
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

    // IClassFactory methods
    STDMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject)
    {
        if (pUnkOuter != nullptr)
            return CLASS_E_NOAGGREGATION;

        // Create instance of your COM object here and return the interface pointer
        // Replace YourComClass with the name of your COM class
        YourComClass* pObj = new YourComClass();
        //int* pObj = nullptr;
        if (!pObj)
            return E_OUTOFMEMORY;

        HRESULT hr = pObj->QueryInterface(riid, ppvObject);
        pObj->Release(); // Release the reference added by QueryInterface
        return hr;
        //return S_OK;
    }

    STDMETHODIMP LockServer(BOOL)
    {
        // Implement if you need to manage the lifetime of your COM server
        return S_OK;
    }
};