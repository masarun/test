#include "pch.h"
#include "MyMFT.h"
#include "MyMFT.tmh"

#include <string>
#include <array>
using namespace std;

MyMFT::MyMFT() : m_cRef(0)
{
    m_InputPinCount = 0;
    m_OutputPinCount = 0;
    m_spSourceTransform = nullptr;

    InitializeCriticalSection(&m_critSec);

    MFCreateEventQueue(&m_pQueue);
}

MyMFT::~MyMFT()
{
    for (UINT index = 0; index < m_basePinArrayInPins.size(); index++)
    {
        if (m_basePinArrayInPins[index] != NULL)
        {
            delete(m_basePinArrayInPins[index]);
        }
    }
    m_basePinArrayInPins.clear();

    for (DWORD index = 0; index < m_basePinArrayOutPins.size(); index++)
    {
        if (m_basePinArrayOutPins[index] != NULL)
        {
            delete(m_basePinArrayOutPins[index]);
        }
    }
    m_basePinArrayOutPins.clear();

    m_spSourceTransform = nullptr;
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
    InterlockedIncrement(&m_cRef);

    return m_cRef;
}

STDMETHODIMP_(ULONG __stdcall) MyMFT::Release()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::Release");
    InterlockedDecrement(&m_cRef);

    if (m_cRef)
    {
        return m_cRef;
    }

    delete this;

    return 0;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::RegisterThreadsEx(DWORD* pdwTaskIndex, LPCWSTR wszClassName, LONG lBasePriority)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::RegisterThreadsEx E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetWorkQueueEx(DWORD dwMultithreadedWorkQueueId, LONG lWorkItemBasePriority)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetWorkQueueEx E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::UnregisterThreads()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::UnregisterThreads E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsEvent(PKSEVENT Event, ULONG EventLength, LPVOID EventData, ULONG DataLength, ULONG* BytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsEvent E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsMethod(PKSMETHOD Method, ULONG MethodLength, LPVOID MethodData, ULONG DataLength, ULONG* BytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsMethod E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsProperty(PKSPROPERTY Property, ULONG PropertyLength, LPVOID PropertyData, ULONG DataLength, ULONG* BytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsProperty E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetAllocatorUsage(DWORD dwOutputStreamID, DWORD* pdwInputStreamID, MFSampleAllocatorUsage* peUsage)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetAllocatorUsage E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetDefaultAllocator(DWORD dwOutputStreamID, IUnknown* pAllocator)
{
TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetDefaultAllocator E_NOTIMPL");
return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::FlushInputStream(DWORD dwStreamIndex, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::FlushInputStream E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::FlushOutputStream(DWORD dwStreamIndex, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::FlushOutputStream E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputAvailableType(DWORD dwInputStreamID, DWORD dwTypeIndex, IMFMediaType** pMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputAvailableType E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputCurrentType(DWORD dwInputStreamID, IMFMediaType** pMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputCurrentType E_NOTIMPL");
    return E_NOTIMPL;
}



STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamAttributes(DWORD dwInputStreamID, IMFAttributes** ppAttributes)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamAttributes Entry");

    HRESULT hr = S_OK;
    ComPtr<CInPin> inPin;

    *ppAttributes = nullptr;

    inPin = GetInPin(dwInputStreamID);

    hr = inPin->QueryInterface(IID_PPV_ARGS(ppAttributes));

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamAttributes Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamPreferredState(DWORD dwStreamID, DeviceStreamState* value, IMFMediaType** ppMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamPreferredState E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamState(DWORD dwStreamID, DeviceStreamState* value)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamState E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputAvailableType(DWORD dwOutputStreamID, DWORD dwTypeIndex, IMFMediaType** pMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputAvailableType Entry");

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputAvailableType dwOutputStreamID: %d", dwOutputStreamID);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputAvailableType dwTypeIndex: %d", dwTypeIndex);

    HRESULT hr = E_FAIL;

    if (pMediaType != NULL)
    {
        ComPtr<COutPin> outPin = GetOutPin(dwOutputStreamID);
        if (outPin != NULL)
        {
            *pMediaType = nullptr;

            hr = outPin->GetOutputAvailableType(dwTypeIndex, pMediaType);
        }
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputAvailableType Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputCurrentType(DWORD dwOutputStreamID, IMFMediaType** pMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputCurrentType E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStreamAttributes(DWORD dwOutputStreamID, IMFAttributes** ppAttributes)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamAttributes Entry");

    HRESULT hr = S_OK;
    ComPtr<COutPin> outPin = nullptr;

    *ppAttributes = nullptr;

    outPin = GetOutPin(dwOutputStreamID);

    hr = outPin->QueryInterface(IID_PPV_ARGS(ppAttributes));

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamAttributes Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStreamState(DWORD dwStreamID, DeviceStreamState* value)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamState E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetStreamCount(DWORD* pcInputStreams, DWORD* pcOutputStreams)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamCount Entry");

    if (pcInputStreams != NULL)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamCount m_InputPinCount: %d", m_InputPinCount);
        *pcInputStreams = m_InputPinCount;
    }

    if (pcOutputStreams != NULL)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamCount m_OutputPinCount: %d", m_OutputPinCount);
        *pcOutputStreams = m_OutputPinCount;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamCount Exit");
    return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetStreamIDs(DWORD dwInputIDArraySize, DWORD* pdwInputStreamIds, DWORD dwOutputIDArraySize, DWORD* pdwOutputStreamIds)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamIDs Entry");

    HRESULT hr = S_OK;

    if ((dwInputIDArraySize < m_InputPinCount) && (dwOutputIDArraySize < m_OutputPinCount))
    {
        hr = E_FAIL;
    }
    else
    {
        if (dwInputIDArraySize)
        {
            for (UINT index = 0; index < dwInputIDArraySize; index++)
            {
                if (m_basePinArrayInPins[index] != NULL)
                {
                    pdwInputStreamIds[index] = m_basePinArrayInPins[index]->GetStreamId();
                    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamIDs m_basePinArrayInPins[%d]->GetStreamId(): %d", index, m_basePinArrayInPins[index]->GetStreamId());
                }
            }
        }

        if (dwOutputIDArraySize)
        {
            for (DWORD index = 0; index < dwOutputIDArraySize; index++)
            {
                if (m_basePinArrayOutPins[index] != NULL)
                {
                    pdwOutputStreamIds[index] = m_basePinArrayOutPins[index]->GetStreamId();
                    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamIDs m_basePinArrayOutPins[%d]->GetStreamId(): %d", index, m_basePinArrayOutPins[index]->GetStreamId());
                }
            }
        }
    }
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamIDs Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::InitializeTransform(_In_ IMFAttributes* pAttributes)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform Entry");

    HRESULT hr = E_FAIL;

    ComPtr<IUnknown> spFilterUnk = nullptr;
    //ComPtr<IMFTransform> spSourceTransform = nullptr;
    DWORD cInputStreams = 0;
    DWORD cOutputStreams = 0;
    DWORD* pcInputStreams = NULL;
    DWORD* pcOutputStreams = NULL;


    hr = pAttributes->GetUnknown(MF_DEVICEMFT_CONNECTED_FILTER_KSCONTROL, IID_PPV_ARGS(&spFilterUnk));
    if (FAILED(hr))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform GetUnknown Failed %!HRESULT!", hr);
        return hr;
    }

    hr = spFilterUnk.As(&m_spSourceTransform);
    if (FAILED(hr))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform spFilterUnk.As Failed %!HRESULT!", hr);
        spFilterUnk = nullptr;
        return hr;
    }

    hr = m_spSourceTransform->GetStreamCount(&cInputStreams, &cOutputStreams);
    if (FAILED(hr))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform GetStreamCount Failed %!HRESULT!", hr);
        spFilterUnk = nullptr;
        return hr;
    }
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform cInputStreams: %d", cInputStreams);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform cOutputStreams: %d", cOutputStreams);

    if ((cInputStreams > 0) || (cOutputStreams > 0))
    {
        m_InputPinCount = cInputStreams;
        m_OutputPinCount = cOutputStreams;

        pcInputStreams = new DWORD[cInputStreams];
        pcOutputStreams = new DWORD[cOutputStreams];
        hr = m_spSourceTransform->GetStreamIDs(cInputStreams, pcInputStreams, cOutputStreams, pcOutputStreams);
        if (FAILED(hr))
        {
            goto CLEANUP;
        }

        for (DWORD index = 0; index < cOutputStreams; index++)
        {
            // Get DevProxy's Output Stream Attributes.
            ComPtr<IMFAttributes> spAttributes;
            hr = m_spSourceTransform.Get()->GetOutputStreamAttributes(index, &spAttributes);
            // hr = spSourceTransform->GetOutputStreamAttributes(index, &spAttributes);
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform m_spSourceTransform.Get()->GetOutputStreamAttributes %!HRESULT!", hr);

            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform pcOutputStreams[%d]: %d", index, pcOutputStreams[index]);

            // Create Input Pin.

            CInPin* inPin = new CInPin(spAttributes.Get(), pcOutputStreams[index]);
            if (inPin != NULL)
            {
                TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform new CInPin(spAttributes.Get(), pcOutputStreams[index]");

                inPin->Init(m_spSourceTransform.Get());
                TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform inPin->Init");

                m_basePinArrayInPins.push_back(inPin);

                TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform m_basePinArrayInPins.push_back(inPin)");
            }
        }

        for (DWORD index = 0; index < m_basePinArrayInPins.size(); index++)
        {
            // Create Output Pin.

            ComPtr<CInPin> inPin = GetInPin(index);
            if (inPin != NULL)
            {
                TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform GetInPin");

                // Get IKSControl from Input Pin.

                ComPtr<IKsControl> ksControl = nullptr;
                inPin.As(&ksControl);
                if (ksControl != NULL)
                {
                    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform inPin.As");

                    // Get Stream Category GUID from Input Pin.
                    GUID guid = GUID_NULL;
                    inPin->GetGUID(MF_DEVICESTREAM_STREAM_CATEGORY, &guid);
                    if (guid != GUID_NULL)
                    {
                        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform inPin->GetGUID");

                        COutPin* outPin = new COutPin(index, m_spSourceTransform.Get(), ksControl.Get());
                        if (outPin != NULL)
                        {
                            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform new COutPin");

                            // Set Stream Category GUID.
                            outPin->SetGUID(MF_DEVICESTREAM_STREAM_CATEGORY, guid);
                            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform outPin->SetGUID");

                            // Set Stream ID.
                            outPin->SetUINT32(MF_DEVICESTREAM_STREAM_ID, index);
                            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform outPin->SetUINT32");

                            // Connect In-Out relationship.
                            BridgeInputPinOutputPin(inPin.Get(), outPin);
                            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform BridgeInputPinOutputPin");

                            m_basePinArrayOutPins.push_back(outPin);
                            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform m_basePinArrayOutPins.push_back(outPin)");

                        }
                    }

                }

            }

        }
    }

CLEANUP:
    if (pcInputStreams != NULL)
    {
        delete[] pcInputStreams;
    }
    if (pcOutputStreams != NULL)
    {
        delete[] pcOutputStreams;
    }

    if (spFilterUnk != nullptr)
    {
        spFilterUnk = nullptr;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessEvent(DWORD dwInputStreamID, IMFMediaEvent* pEvent)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessEvent E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessInput(DWORD dwInputStreamID, IMFSample* pSample, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessInput E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessMessage(MFT_MESSAGE_TYPE eMessage, ULONG_PTR ulParam)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, MFT_OUTPUT_DATA_BUFFER* pOutputSample, DWORD* pdwStatus)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessOutput E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetInputStreamState(DWORD dwStreamID, IMFMediaType* pMediaType, DeviceStreamState value, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetInputStreamState E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetOutputStreamState(DWORD dwStreamID, IMFMediaType* pMediaType, DeviceStreamState value, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetOutputStreamState E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(VOID __stdcall) MyMFT::FlushAllStreams(VOID)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::FlushAllStreams E_NOTIMPL");
    return VOID();
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetShutdownStatus(MFSHUTDOWN_STATUS* pStatus)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetShutdownStatus E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::Shutdown()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::Shutdown Entry");

    HRESULT hr = E_FAIL;

    EnterCriticalSection(&m_critSec);

    if (m_pQueue)
    {
        hr = m_pQueue->Shutdown();
        
        m_pQueue->Release();
        m_pQueue = NULL;
    }

    LeaveCriticalSection(&m_critSec);

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::Shutdown Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::BeginGetEvent(IMFAsyncCallback* pCallback, IUnknown* punkState)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BeginGetEvent Entry");

    HRESULT hr = E_FAIL;

    EnterCriticalSection(&m_critSec);

    if (m_pQueue)
    {
        hr = m_pQueue->BeginGetEvent(pCallback, punkState);
    }

    LeaveCriticalSection(&m_critSec);

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BeginGetEvent Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::EndGetEvent Entry");

    HRESULT hr = E_FAIL;

    EnterCriticalSection(&m_critSec);

    if (m_pQueue)
    {
        hr = m_pQueue->EndGetEvent(pResult, ppEvent);
    }

    LeaveCriticalSection(&m_critSec);

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::EndGetEvent Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetEvent(DWORD dwFlags, IMFMediaEvent** ppEvent)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetEvent Entry");

    HRESULT hr = E_FAIL;

    IMFMediaEventQueue* pQueue = NULL;
    
    EnterCriticalSection(&m_critSec);

    if (m_pQueue)
    {
        pQueue = m_pQueue;
        pQueue->AddRef();
    }

    LeaveCriticalSection(&m_critSec);

    if (pQueue)
    {
        pQueue->GetEvent(dwFlags, ppEvent);

        pQueue->Release();
        pQueue = NULL;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::EndGetEvent Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::QueueEvent(MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT* pvValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueueEvent Entry");

    HRESULT hr = E_FAIL;

    EnterCriticalSection(&m_critSec);
    
    if (m_pQueue)
    {
        hr = m_pQueue->QueueEventParamVar(met, guidExtendedType, hrStatus, pvValue);
    }

    LeaveCriticalSection(&m_critSec);

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueueEvent Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::AddInputStreams(DWORD cStreams, DWORD* adwStreamIDs)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::AddInputStreams E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::DeleteInputStream(DWORD dwStreamID)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::DeleteInputStream E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetAttributes(IMFAttributes** pAttributes)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetAttributes E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStatus(DWORD dwInputStreamID, DWORD* pdwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStatus E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamInfo(DWORD dwInputStreamID, MFT_INPUT_STREAM_INFO* pStreamInfo)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamInfo E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStatus(DWORD* pdwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStatus E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStreamInfo(DWORD dwOutputStreamID, MFT_OUTPUT_STREAM_INFO* pStreamInfo)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamInfo E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetStreamLimits(DWORD* pdwInputMinimum, DWORD* pdwInputMaximum, DWORD* pdwOutputMinimum, DWORD* pdwOutputMaximum)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamLimits E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetInputType(DWORD dwInputStreamID, IMFMediaType* pType, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetInputType E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetOutputBounds(LONGLONG hnsLowerBound, LONGLONG hnsUpperBound)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetOutputBounds E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetOutputType(DWORD dwOutputStreamID, IMFMediaType* pType, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetOutputType E_NOTIMPL");
    return E_NOTIMPL;
}

CInPin* MyMFT::GetInPin(DWORD dwStreamId)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInPin Entry");

    CInPin* pin = nullptr;

    for (UINT index = 0; index < m_basePinArrayInPins.size(); index++)
    {
        if (m_basePinArrayInPins[index]->GetStreamId() == dwStreamId)
        {
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "m_basePinArrayInPins[%d]->GetStreamId() == %d", index, dwStreamId);

            pin = (CInPin *)m_basePinArrayInPins[index];
            break;
        }
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInPin Exit");
    return pin;
}

COutPin* MyMFT::GetOutPin(DWORD dwStreamId)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutPin Entry");

    COutPin* pin = nullptr;

    for (DWORD index = 0; index < m_basePinArrayOutPins.size(); index++)
    {
        if (m_basePinArrayOutPins[index]->GetStreamId() == dwStreamId)
        {
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "m_basePinArrayOutPins[%d]->GetStreamId() == %d", index, dwStreamId);

            pin = (COutPin*)m_basePinArrayOutPins[index];
            break;
        }
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutPin Exit");
    return pin;
}

HRESULT MyMFT::BridgeInputPinOutputPin(CInPin* pInPin, COutPin* pOutPin)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BridgeInputPinOutputPin Entry");

    HRESULT hr = E_FAIL;

    if ((pInPin != NULL) && (pOutPin != NULL))
    {
        DWORD index = 0;
        DWORD addedMediaTypeCount = 0;
        ComPtr<IMFMediaType> mediaType;

        while (SUCCEEDED(hr = pInPin->GetMediaTypeAt(index++, mediaType.ReleaseAndGetAddressOf())))
        {
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BridgeInputPinOutputPin pInPin->GetMediaTypeAt");

            GUID subType = GUID_NULL;
            hr = mediaType->GetGUID(MF_MT_SUBTYPE, &subType);
            if (FAILED(hr))
            {
                break;
            }
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BridgeInputPinOutputPin mediaType->GetGUID");

            hr = pOutPin->AddMediaType(NULL, mediaType.Get());
            if (hr == S_OK)
            {
                TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BridgeInputPinOutputPin pOutPin->AddMediaType");
                addedMediaTypeCount++;
            }
        }

        if (addedMediaTypeCount == 0)
        {
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BridgeInputPinOutputPin addedMediaTypeCount == 0");

            hr = MF_E_INVALID_STREAM_DATA;
        }

    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BridgeInputPinOutputPin Exit %!HRESULT!", hr);
    return hr;
}