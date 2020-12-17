#include "pch.h"
#include "MyMFT.h"

STDMETHODIMP_(HRESULT __stdcall) MyMFT::QueryInterface(REFIID, VOID**)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(ULONG __stdcall) MyMFT::AddRef()
{
    return 0;
}

STDMETHODIMP_(ULONG __stdcall) MyMFT::Release()
{
    return 0;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::RegisterThreadsEx(DWORD* pdwTaskIndex, LPCWSTR wszClassName, LONG lBasePriority)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetWorkQueueEx(DWORD dwMultithreadedWorkQueueId, LONG lWorkItemBasePriority)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::UnregisterThreads()
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsEvent(PKSEVENT Event, ULONG EventLength, LPVOID EventData, ULONG DataLength, ULONG* BytesReturned)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsMethod(PKSMETHOD Method, ULONG MethodLength, LPVOID MethodData, ULONG DataLength, ULONG* BytesReturned)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsProperty(PKSPROPERTY Property, ULONG PropertyLength, LPVOID PropertyData, ULONG DataLength, ULONG* BytesReturned)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetAllocatorUsage(DWORD dwOutputStreamID, DWORD* pdwInputStreamID, MFSampleAllocatorUsage* peUsage)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetDefaultAllocator(DWORD dwOutputStreamID, IUnknown* pAllocator)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::FlushInputStream(DWORD dwStreamIndex, DWORD dwFlags)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::FlushOutputStream(DWORD dwStreamIndex, DWORD dwFlags)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputAvailableType(DWORD dwInputStreamID, DWORD dwTypeIndex, IMFMediaType** pMediaType)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputCurrentType(DWORD dwInputStreamID, IMFMediaType** pMediaType)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamAttributes(DWORD dwInputStreamID, IMFAttributes** ppAttributes)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamPreferredState(DWORD dwStreamID, DeviceStreamState* value, IMFMediaType** ppMediaType)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamState(DWORD dwStreamID, DeviceStreamState* value)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputAvailableType(DWORD dwOutputStreamID, DWORD dwTypeIndex, IMFMediaType** pMediaType)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputCurrentType(DWORD dwOutputStreamID, IMFMediaType** pMediaType)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStreamAttributes(DWORD dwOutputStreamID, IMFAttributes** ppAttributes)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStreamState(DWORD dwStreamID, DeviceStreamState* value)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetStreamCount(DWORD* pcInputStreams, DWORD* pcOutputStreams)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetStreamIDs(DWORD dwInputIDArraySize, DWORD* pdwInputStreamIds, DWORD dwOutputIDArraySize, DWORD* pdwOutputStreamIds)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::InitializeTransform(IMFAttributes* pAttributes)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessEvent(DWORD dwInputStreamID, IMFMediaEvent* pEvent)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessInput(DWORD dwInputStreamID, IMFSample* pSample, DWORD dwFlags)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessMessage(MFT_MESSAGE_TYPE eMessage, ULONG_PTR ulParam)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, MFT_OUTPUT_DATA_BUFFER* pOutputSample, DWORD* pdwStatus)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetInputStreamState(DWORD dwStreamID, IMFMediaType* pMediaType, DeviceStreamState value, DWORD dwFlags)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetOutputStreamState(DWORD dwStreamID, IMFMediaType* pMediaType, DeviceStreamState value, DWORD dwFlags)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetShutdownStatus(MFSHUTDOWN_STATUS* pStatus)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::Shutdown()
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::BeginGetEvent(IMFAsyncCallback* pCallback, IUnknown* punkState)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetEvent(DWORD dwFlags, IMFMediaEvent** ppEvent)
{
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT* pvValue)
{
    return E_NOTIMPL;
}
