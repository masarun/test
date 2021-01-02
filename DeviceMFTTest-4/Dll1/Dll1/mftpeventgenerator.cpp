//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media Foundation
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//

#include "pch.h"
#include "stdafx.h"
#include "common.h"
#include "mftpeventgenerator.h"



#ifdef MF_WPP
#include "mftpeventgenerator.tmh"    //--REF_ANALYZER_DONT_REMOVE--
#endif

CMediaEventGenerator::CMediaEventGenerator () :
    m_nRefCount(0),
    m_pQueue(NULL),
    m_bShutdown(FALSE)
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMediaEventGenerator::CMediaEventGenerator");

    //Call this explicit...
    InitMediaEventGenerator();
}

STDMETHODIMP CMediaEventGenerator::InitMediaEventGenerator(
    void
    )
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMediaEventGenerator::InitMediaEventGenerator");

    return MFCreateEventQueue(&m_pQueue);

}

STDMETHODIMP_(ULONG) CMediaEventGenerator::AddRef(
    void
    )
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMediaEventGenerator::AddRef");

    return InterlockedIncrement(&m_nRefCount);
}

STDMETHODIMP_(ULONG) CMediaEventGenerator::Release(
    void
    )
{
    ULONG uCount = InterlockedDecrement(&m_nRefCount);

    if (uCount == 0)
    {
        delete this;
    }
    return uCount;
}

STDMETHODIMP CMediaEventGenerator::QueryInterface(
    _In_ REFIID iid,
    _COM_Outptr_ void** ppv)
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMediaEventGenerator::QueryInterface");

    HRESULT hr = S_OK;

    *ppv = NULL;

    if (iid == __uuidof(IUnknown) || iid == __uuidof(IMFMediaEventGenerator))
    {
        *ppv = static_cast<IMFMediaEventGenerator*>(this);
        AddRef();
    }
    else
    {
        hr = E_NOINTERFACE;
    }

    return hr;
}

//
// IMediaEventGenerator methods
//
STDMETHODIMP CMediaEventGenerator::BeginGetEvent(
    _In_ IMFAsyncCallback* pCallback,
    _In_ IUnknown* pState
    )
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMediaEventGenerator::BeginGetEvent -----");

    HRESULT hr = S_OK;
    m_critSec.Lock();

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = m_pQueue->BeginGetEvent(pCallback, pState);
    }

    m_critSec.Unlock();

    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;    

}

STDMETHODIMP CMediaEventGenerator::EndGetEvent(
    _In_ IMFAsyncResult* pResult,
    _Outptr_result_maybenull_ IMFMediaEvent** ppEvent
    )
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMediaEventGenerator::EndGetEvent -----");

    HRESULT hr = S_OK;
    m_critSec.Lock();

    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = m_pQueue->EndGetEvent(pResult, ppEvent);
    }
    
    m_critSec.Unlock();
    
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;    

}

STDMETHODIMP CMediaEventGenerator::GetEvent(
    _In_ DWORD dwFlags,
    _Outptr_result_maybenull_ IMFMediaEvent** ppEvent
    )
{
    UNREFERENCED_PARAMETER(ppEvent);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMediaEventGenerator::GetEvent -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMediaEventGenerator::GetEvent dwFlags: %d", dwFlags);
    return E_NOTIMPL;

}

STDMETHODIMP CMediaEventGenerator::QueueEvent(
    _In_ MediaEventType met,
    _In_ REFGUID extendedType,
    _In_ HRESULT hrStatus,
    _In_opt_ const PROPVARIANT* pvValue
    )
{
    UNREFERENCED_PARAMETER(met);
    UNREFERENCED_PARAMETER(extendedType);
    UNREFERENCED_PARAMETER(hrStatus);
    UNREFERENCED_PARAMETER(pvValue);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMediaEventGenerator::QueueEvent 1 E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP CMediaEventGenerator::ShutdownEventGenerator(
    void
    )
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMediaEventGenerator::ShutdownEventGenerator");
    return E_NOTIMPL;
}

STDMETHODIMP CMediaEventGenerator::QueueEvent(
    _In_ IMFMediaEvent* pEvent
    )
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMediaEventGenerator::QueueEvent 2 -----");

    HRESULT hr = S_OK;
    m_critSec.Lock();
    
    hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        if (m_pQueue)
        {
            hr = m_pQueue->QueueEvent(pEvent);
        }
    }

    m_critSec.Unlock();

    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;

}

CMediaEventGenerator::~CMediaEventGenerator (
    void
    )
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMediaEventGenerator::~CMediaEventGenerator");

    ShutdownEventGenerator();
}
