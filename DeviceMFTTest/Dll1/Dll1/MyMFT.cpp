#include "pch.h"
#include "MyMFT.h"
#include "MyMFT.tmh"

#include <string>
#include <array>
using namespace std;

MyMFT::MyMFT()
{
}

// https://stackoverflow.com/questions/21431047/how-to-convert-guid-to-char?rq=1
std::string guidToString(GUID guid) {
    std::array<char, 40> output;
    snprintf(output.data(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    return std::string(output.data());
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::QueryInterface(REFIID rIID, VOID** ppInterface)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueryInterface Entry");

    if (rIID == __uuidof(IUnknown))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueryInterface IUnknown");
        *ppInterface = static_cast<IMFDeviceTransform *>(this);
    }
    else if (rIID == __uuidof(IMFDeviceTransform))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueryInterface IMFDeviceTransform");
        *ppInterface = static_cast<IMFDeviceTransform*>(this);
    }
    else if (rIID == __uuidof(IMFTransform))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CMultipinMft::QueryInterface IMFTransform");
        *ppInterface = static_cast<IMFTransform*>(this);
    }
    else if (rIID == __uuidof(IMFMediaEventGenerator))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueryInterface IMFMediaEventGenerator");
        *ppInterface = static_cast<IMFMediaEventGenerator*>(this);
    }
    else if (rIID == __uuidof(IMFShutdown))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueryInterface IMFShutdown");
        *ppInterface = static_cast<IMFShutdown*>(this);
    }
    else if (rIID == __uuidof(IKsControl))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueryInterface IKsControl");
        *ppInterface = static_cast<IKsControl*>(this);
    }
    else if (rIID == __uuidof(IMFRealTimeClientEx))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueryInterface IMFRealTimeClientEx");
        *ppInterface = static_cast<IMFRealTimeClientEx*>(this);
    }
    else
    {
        string strGuid = guidToString(rIID);
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueryInterface E_NOINTERFACE %s", strGuid.c_str());

        *ppInterface = NULL;

        return E_NOINTERFACE;
    }

    ((LPUNKNOWN)*ppInterface)->AddRef();

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueryInterface Exit");
    return NOERROR;
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

STDMETHODIMP_(HRESULT __stdcall) MyMFT::InitializeTransform(_In_ IMFAttributes* pAttributes)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform");
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

STDMETHODIMP_(VOID __stdcall) MyMFT::FlushAllStreams(VOID)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::FlushAllStreams");
    return VOID();
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

STDMETHODIMP_(HRESULT __stdcall) MyMFT::AddInputStreams(DWORD cStreams, DWORD* adwStreamIDs)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::AddInputStreams");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::DeleteInputStream(DWORD dwStreamID)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::DeleteInputStream");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetAttributes(IMFAttributes** pAttributes)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetAttributes");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStatus(DWORD dwInputStreamID, DWORD* pdwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStatus");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamInfo(DWORD dwInputStreamID, MFT_INPUT_STREAM_INFO* pStreamInfo)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamInfo");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStatus(DWORD* pdwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStatus");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStreamInfo(DWORD dwOutputStreamID, MFT_OUTPUT_STREAM_INFO* pStreamInfo)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamInfo");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetStreamLimits(DWORD* pdwInputMinimum, DWORD* pdwInputMaximum, DWORD* pdwOutputMinimum, DWORD* pdwOutputMaximum)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamLimits");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetInputType(DWORD dwInputStreamID, IMFMediaType* pType, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetInputType");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetOutputBounds(LONGLONG hnsLowerBound, LONGLONG hnsUpperBound)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetOutputBounds");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetOutputType(DWORD dwOutputStreamID, IMFMediaType* pType, DWORD dwFlags)
{

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetOutputType");
    return E_NOTIMPL;
}
