#include "pch.h"
#include "Pin.h"
#include "Pin.tmh"

STDMETHODIMP_(HRESULT __stdcall) CBasePin::QueryInterface(REFIID, VOID**)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::QueryInterface -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(ULONG __stdcall) CBasePin::AddRef()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::AddRef -----");
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG __stdcall) CBasePin::Release()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::Release -----");

    ULONG count = InterlockedDecrement(&m_cRef);

    if (count == 0)
    {
        delete this;
    }

    return count;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::KsEvent(PKSEVENT, ULONG, LPVOID, ULONG, ULONG*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::KsEvent E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::KsMethod(PKSMETHOD, ULONG, LPVOID, ULONG, ULONG*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::KsMethod E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::KsProperty(PKSPROPERTY, ULONG, LPVOID, ULONG, ULONG*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::KsProperty E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::Compare(IMFAttributes*, MF_ATTRIBUTES_MATCH_TYPE, BOOL*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::Compare E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::CompareItem(REFGUID, REFPROPVARIANT, BOOL*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::CompareItem E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::CopyAllItems(IMFAttributes*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::CopyAllItems E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::DeleteAllItems()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::DeleteAllItems E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::DeleteItem(REFGUID)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::DeleteItem E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetAllocatedBlob(REFGUID, UINT8**, UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetAllocatedBlob E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetAllocatedString(REFGUID, LPWSTR*, UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetAllocatedString E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetBlob(REFGUID, UINT8*, UINT32, UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetBlob E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetBlobSize(REFGUID, UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetBlobSize E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetCount(UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetCount E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetDouble(REFGUID, double*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetDouble E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetGUID(REFGUID, GUID*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetGUID E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetItem(REFGUID, PROPVARIANT*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetItem E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetItemByIndex(UINT32, GUID*, PROPVARIANT*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetItemByIndex E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetItemType(REFGUID, MF_ATTRIBUTE_TYPE*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetItemType E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetString(REFGUID, LPWSTR, UINT32, UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetString E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetStringLength(REFGUID, UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetStringLength E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetUINT32(REFGUID, UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetUINT32 E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetUINT64(REFGUID, UINT64*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetUINT64 E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetUnknown(REFGUID, REFIID, LPVOID*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetUnknown E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::LockStore()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::LockStore E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetBlob(REFGUID, const UINT8*, UINT32)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetBlob E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetDouble(REFGUID, double)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetDouble E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetGUID(REFGUID, REFGUID)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetGUID E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetItem(REFGUID, REFPROPVARIANT)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetItem E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetString(REFGUID, LPCWSTR)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetString E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetUINT32(REFGUID, UINT32)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetUINT32 E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetUINT64(REFGUID, UINT64)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetUINT64 E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetUnknown(REFGUID, IUnknown*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetUnknown E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::UnlockStore()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::UnlockStore E_NOTIMPL -----");
    return E_NOTIMPL;
}
