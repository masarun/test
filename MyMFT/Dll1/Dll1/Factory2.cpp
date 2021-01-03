#include "pch.h"
#include "MyMFT.h"
#include "Factory2.h"
#include "Fortune2.h"
#include "Counts.h"

ComFortuneTellerFactory::ComFortuneTellerFactory() : m_cRef(0)
{
}

STDMETHODIMP_(HRESULT __stdcall) ComFortuneTellerFactory::QueryInterface(REFIID rIID, VOID** ppInterface)
{
    *ppInterface = NULL;

    if (rIID == IID_IUnknown)
    {
        *ppInterface = this;
    }
    else if (rIID == IID_IClassFactory)
    {
        *ppInterface = this;
    }
    else
    {
        *ppInterface = NULL;

        return E_NOINTERFACE;
    }

    ((LPUNKNOWN)*ppInterface)->AddRef();

    return NOERROR;
}

STDMETHODIMP_(ULONG __stdcall) ComFortuneTellerFactory::AddRef()
{
    m_cRef++;

    return m_cRef;
}

STDMETHODIMP_(ULONG __stdcall) ComFortuneTellerFactory::Release()
{
    m_cRef--;

    if (m_cRef)
    {
        return m_cRef;
    }

    delete this;

    return 0;
}

STDMETHODIMP_(HRESULT __stdcall) ComFortuneTellerFactory::CreateInstance(LPUNKNOWN pUnkOuter, REFIID rIID, VOID** ppInterface)
{
    //PComFortuneTeller pObj;
    MyMFT* pObj;
    HRESULT hr;

    *ppInterface = NULL;

    if (pUnkOuter && (rIID != IID_IUnknown))
    {
        return CLASS_E_NOAGGREGATION;
    }

    //pObj = new ComFortuneTeller();
    pObj = new MyMFT();

    if (!pObj)
    {
        return E_OUTOFMEMORY;
    }

    hr = pObj->QueryInterface(rIID, ppInterface);
    if (FAILED(hr))
    {
        delete pObj;
        return hr;
    }

    Counters::IncObjectCount();

    return NOERROR;
}

STDMETHODIMP_(HRESULT __stdcall) ComFortuneTellerFactory::LockServer(BOOL fLock)
{
    if (fLock)
    {
        Counters::IncLockCount();
    }
    else
    {
        Counters::DecLockCount();
    }

    return NOERROR;
}
