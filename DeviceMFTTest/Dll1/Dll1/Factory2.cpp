#include "pch.h"
#include "MyMFT.h"
#include "Factory2.tmh"
#include "Factory2.h"
#include "Fortune2.h"
#include "Counts.h"

ComFortuneTellerFactory::ComFortuneTellerFactory() : m_cRef(0)
{
}

STDMETHODIMP_(HRESULT __stdcall) ComFortuneTellerFactory::QueryInterface(REFIID rIID, VOID** ppInterface)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::QueryInterface Entry");

    *ppInterface = NULL;

    if (rIID == IID_IUnknown)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::QueryInterface rIID == IID_IUnknown");

        *ppInterface = this;
    }
    else if (rIID == IID_IClassFactory)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::QueryInterface rIID == IID_IClassFactory");

        *ppInterface = this;
    }
    else
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::QueryInterface E_NOINTERFACE");

        *ppInterface = NULL;

        return E_NOINTERFACE;
    }

    ((LPUNKNOWN)*ppInterface)->AddRef();

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::QueryInterface Exit");
    return NOERROR;
}

STDMETHODIMP_(ULONG __stdcall) ComFortuneTellerFactory::AddRef()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::AddRef");

    m_cRef++;

    return m_cRef;
}

STDMETHODIMP_(ULONG __stdcall) ComFortuneTellerFactory::Release()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::Release");

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
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::CreateInstance Entry");

    //PComFortuneTeller pObj;
    PMyMFT pObj;
    HRESULT hr;

    *ppInterface = NULL;

    if (pUnkOuter && (rIID != IID_IUnknown))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::CreateInstance CLASS_E_NOAGGREGATION");
        return CLASS_E_NOAGGREGATION;
    }

    //pObj = new ComFortuneTeller();
    pObj = new MyMFT();

    if (!pObj)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::CreateInstance E_OUTOFMEMORY");

        return E_OUTOFMEMORY;
    }

    hr = pObj->QueryInterface(rIID, ppInterface);
    if (FAILED(hr))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::CreateInstance QueryInterface failed");

        delete pObj;
        return hr;
    }

    Counters::IncObjectCount();

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::CreateInstance Exit");
    return NOERROR;
}

STDMETHODIMP_(HRESULT __stdcall) ComFortuneTellerFactory::LockServer(BOOL fLock)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::LockServer");

    if (fLock)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::LockServer IncLockCount");

        Counters::IncLockCount();
    }
    else
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "ComFortuneTellerFactory::LockServer DecLockCount");

        Counters::DecLockCount();
    }

    return NOERROR;
}
