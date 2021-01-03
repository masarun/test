#include "pch.h"
#include "MyMFT.h"
#include "MyMFT.tmh"

#include <string>
#include <array>
using namespace std;

// https://stackoverflow.com/questions/21431047/how-to-convert-guid-to-char?rq=1
std::string guidToString(GUID guid) {
    std::array<char, 40> output;
    snprintf(output.data(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    return std::string(output.data());
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::QueryInterface(REFIID rIID, VOID** ppInterface)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueryInterface Entry -----");
    if (rIID == __uuidof(IUnknown))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueryInterface IUnknown");
        *ppInterface = static_cast<IMFDeviceTransform*>(this);
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

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueryInterface Exit -----");
    return NOERROR;
}

STDMETHODIMP_(ULONG __stdcall) MyMFT::AddRef()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::AddRef -----");
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG __stdcall) MyMFT::Release()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::Release -----");

    ULONG count = InterlockedDecrement(&m_cRef);

    if (count == 0)
    {
        delete this;
    }

    return count;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::RegisterThreadsEx(DWORD*, LPCWSTR, LONG)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::RegisterThreadsEx E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetWorkQueueEx(DWORD, LONG)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetWorkQueueEx E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::UnregisterThreads()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::UnregisterThreads E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsEvent(PKSEVENT, ULONG, LPVOID, ULONG, ULONG*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsEvent E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsMethod(PKSMETHOD, ULONG, LPVOID, ULONG, ULONG*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsMethod E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsProperty(PKSPROPERTY, ULONG, LPVOID, ULONG, ULONG*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsProperty E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetAllocatorUsage(DWORD, DWORD*, MFSampleAllocatorUsage*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetAllocatorUsage E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetDefaultAllocator(DWORD, IUnknown*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetDefaultAllocator E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::FlushInputStream(DWORD, DWORD)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::FlushInputStream E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::FlushOutputStream(DWORD, DWORD)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::FlushOutputStream E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputAvailableType(DWORD, DWORD, IMFMediaType**)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputAvailableType E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputCurrentType(DWORD, IMFMediaType**)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputCurrentType E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamAttributes(DWORD, IMFAttributes**)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamAttributes E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamPreferredState(DWORD, DeviceStreamState*, IMFMediaType**)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamPreferredState E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamState(DWORD, DeviceStreamState*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamState E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputAvailableType(DWORD, DWORD, IMFMediaType**)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputAvailableType E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputCurrentType(DWORD, IMFMediaType**)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputCurrentType E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStreamAttributes(DWORD, IMFAttributes**)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamAttributes E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStreamState(DWORD, DeviceStreamState*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamState E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetStreamCount(DWORD*, DWORD*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamCount E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetStreamIDs(DWORD, DWORD*, DWORD, DWORD*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamIDs E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::InitializeTransform(IMFAttributes*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessEvent(DWORD, IMFMediaEvent*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessEvent E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessInput(DWORD, IMFSample*, DWORD)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessInput E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessMessage(MFT_MESSAGE_TYPE, ULONG_PTR)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessOutput(DWORD, DWORD, MFT_OUTPUT_DATA_BUFFER*, DWORD*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessOutput E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetInputStreamState(DWORD, IMFMediaType*, DeviceStreamState, DWORD)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetInputStreamState E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetOutputStreamState(DWORD, IMFMediaType*, DeviceStreamState, DWORD)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetOutputStreamState E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(VOID __stdcall) MyMFT::FlushAllStreams(VOID)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::FlushAllStreams NOTIMPL -----");
    return VOID();
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetShutdownStatus(MFSHUTDOWN_STATUS*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetShutdownStatus E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::Shutdown()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::Shutdown E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::BeginGetEvent(IMFAsyncCallback*, IUnknown*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BeginGetEvent E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::EndGetEvent(IMFAsyncResult*, IMFMediaEvent**)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::EndGetEvent E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetEvent(DWORD, IMFMediaEvent**)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetEvent E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::QueueEvent(MediaEventType, REFGUID, HRESULT, const PROPVARIANT*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueueEvent E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::AddInputStreams(DWORD, DWORD*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::AddInputStreams E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::DeleteInputStream(DWORD)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::DeleteInputStream E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetAttributes(IMFAttributes**)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetAttributes E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStatus(DWORD, DWORD*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStatus E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamInfo(DWORD, MFT_INPUT_STREAM_INFO*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamInfo E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStatus(DWORD*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStatus E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStreamInfo(DWORD, MFT_OUTPUT_STREAM_INFO*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamInfo E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetStreamLimits(DWORD*, DWORD*, DWORD*, DWORD*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamLimits E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetInputType(DWORD, IMFMediaType*, DWORD)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetInputType E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetOutputBounds(LONGLONG, LONGLONG)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetOutputBounds E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetOutputType(DWORD, IMFMediaType*, DWORD)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetOutputType E_NOTIMPL -----");
    return E_NOTIMPL;
}
