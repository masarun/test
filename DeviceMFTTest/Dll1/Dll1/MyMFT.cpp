#include "pch.h"
#include "MyMFT.h"
#include "MyMFT.tmh"

STDMETHODIMP_(HRESULT __stdcall) MyMFT::QueryInterface(REFIID, VOID**)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueryInterface");
    return E_NOTIMPL;
}

STDMETHODIMP_(ULONG __stdcall) MyMFT::AddRef()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::AddRef");
    return 0;
}

STDMETHODIMP_(ULONG __stdcall) MyMFT::Release()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::Release");
    return 0;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::RegisterThreadsEx(DWORD* pdwTaskIndex, LPCWSTR wszClassName, LONG lBasePriority)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::RegisterThreadsEx");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetWorkQueueEx(DWORD dwMultithreadedWorkQueueId, LONG lWorkItemBasePriority)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetWorkQueueEx");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::UnregisterThreads()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::UnregisterThreads");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsEvent(PKSEVENT Event, ULONG EventLength, LPVOID EventData, ULONG DataLength, ULONG* BytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsEvent");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsMethod(PKSMETHOD Method, ULONG MethodLength, LPVOID MethodData, ULONG DataLength, ULONG* BytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsMethod");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsProperty(PKSPROPERTY Property, ULONG PropertyLength, LPVOID PropertyData, ULONG DataLength, ULONG* BytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsProperty");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetAllocatorUsage(DWORD dwOutputStreamID, DWORD* pdwInputStreamID, MFSampleAllocatorUsage* peUsage)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetAllocatorUsage");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetDefaultAllocator(DWORD dwOutputStreamID, IUnknown* pAllocator)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetDefaultAllocator");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::FlushInputStream(DWORD dwStreamIndex, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::FlushInputStream");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::FlushOutputStream(DWORD dwStreamIndex, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::FlushOutputStream");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputAvailableType(DWORD dwInputStreamID, DWORD dwTypeIndex, IMFMediaType** pMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputAvailableType");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputCurrentType(DWORD dwInputStreamID, IMFMediaType** pMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputCurrentType");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamAttributes(DWORD dwInputStreamID, IMFAttributes** ppAttributes)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamAttributes");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamPreferredState(DWORD dwStreamID, DeviceStreamState* value, IMFMediaType** ppMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamPreferredState");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamState(DWORD dwStreamID, DeviceStreamState* value)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamState");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputAvailableType(DWORD dwOutputStreamID, DWORD dwTypeIndex, IMFMediaType** pMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputAvailableType");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputCurrentType(DWORD dwOutputStreamID, IMFMediaType** pMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputCurrentType");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStreamAttributes(DWORD dwOutputStreamID, IMFAttributes** ppAttributes)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamAttributes");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStreamState(DWORD dwStreamID, DeviceStreamState* value)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamState");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetStreamCount(DWORD* pcInputStreams, DWORD* pcOutputStreams)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamCount");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetStreamIDs(DWORD dwInputIDArraySize, DWORD* pdwInputStreamIds, DWORD dwOutputIDArraySize, DWORD* pdwOutputStreamIds)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamIDs");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::InitializeTransform(IMFAttributes* pAttributes)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStrInitializeTransformeamIDs");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessEvent(DWORD dwInputStreamID, IMFMediaEvent* pEvent)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessEvent");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessInput(DWORD dwInputStreamID, IMFSample* pSample, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessInput");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessMessage(MFT_MESSAGE_TYPE eMessage, ULONG_PTR ulParam)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, MFT_OUTPUT_DATA_BUFFER* pOutputSample, DWORD* pdwStatus)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessOutput");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetInputStreamState(DWORD dwStreamID, IMFMediaType* pMediaType, DeviceStreamState value, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetInputStreamState");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetOutputStreamState(DWORD dwStreamID, IMFMediaType* pMediaType, DeviceStreamState value, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetOutputStreamState");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetShutdownStatus(MFSHUTDOWN_STATUS* pStatus)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetShutdownStatus");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::Shutdown()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::Shutdown");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::BeginGetEvent(IMFAsyncCallback* pCallback, IUnknown* punkState)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BeginGetEvent");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::EndGetEvent");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetEvent(DWORD dwFlags, IMFMediaEvent** ppEvent)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetEvent");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT* pvValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueueEvent");
    return E_NOTIMPL;
}
