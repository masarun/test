#include "pch.h"
#include "Fortune2.tmh"
#include "Fortune2.h"
#include "Counts.h"

#include <string>
using namespace std;

static wstring g_rgWstrFortunes[] = 
    {
        L"Hoge1",
        L"Hoge2"
    };

static const ULONG g_cWstrFortunes = sizeof(g_rgWstrFortunes) / sizeof(wstring);

ComFortuneTeller::ComFortuneTeller() : m_cRef(0)
{
}

STDMETHODIMP_(HRESULT __stdcall) ComFortuneTeller::QueryInterface(REFIID rIID, VOID** ppInterface)
{
    if (rIID == IID_IUnknown)
    {
        *ppInterface = this;
    }
    else if (rIID == IID_IFortuneTeller)
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

STDMETHODIMP_(ULONG __stdcall) ComFortuneTeller::AddRef()
{
    m_cRef++;

    return m_cRef;
}

STDMETHODIMP_(ULONG __stdcall) ComFortuneTeller::Release()
{
    m_cRef--;

    if (m_cRef)
    {
        return m_cRef;
    }

    Counters::DecObjectCount();

    delete this;

    return 0;
}

STDMETHODIMP_(HRESULT __stdcall) ComFortuneTeller::GetFortune(BSTR* pBstrFortune)
{
    //DoTrace

    *pBstrFortune = NULL;

    *pBstrFortune = SysAllocString(g_rgWstrFortunes[0].c_str());

    return NOERROR;
}
