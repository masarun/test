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
    m_spDeviceManagerUnk = nullptr;

    InitializeCriticalSection(&m_critSec);
    InitializeCriticalSection(&m_critSecForEventGenerator);

    MFCreateEventQueue(&m_pQueue);
}

MyMFT::~MyMFT()
{
    DeleteCriticalSection(&m_critSec);
    DeleteCriticalSection(&m_critSecForEventGenerator);

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
    m_spDeviceManagerUnk = nullptr;
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
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetWorkQueueEx Entry");

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetWorkQueueEx dwMultithreadedWorkQueueId: %d", dwMultithreadedWorkQueueId);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetWorkQueueEx lWorkItemBasePriority: %d", lWorkItemBasePriority);

    DWORD m_dwMultithreadedWorkQueueId = dwMultithreadedWorkQueueId;
    LONG m_lWorkItemBasePriority = lWorkItemBasePriority;

    for (DWORD index = 0; index < m_basePinArrayInPins.size(); index++)
    {
        m_basePinArrayInPins[index]->SetWorkQueue(dwMultithreadedWorkQueueId);
    }

    for (DWORD index = 0; index < m_basePinArrayOutPins.size(); index++)
    {
        m_basePinArrayOutPins[index]->SetWorkQueue(dwMultithreadedWorkQueueId);
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetWorkQueueEx Exit");
    return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::UnregisterThreads()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::UnregisterThreads E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsEvent(PKSEVENT Event, ULONG EventLength, LPVOID EventData, ULONG DataLength, ULONG* BytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsEvent Entry");

    HRESULT hr = m_spIkscontrol->KsEvent(Event, EventLength, EventData, DataLength, BytesReturned);

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsEvent Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsMethod(PKSMETHOD Method, ULONG MethodLength, LPVOID MethodData, ULONG DataLength, ULONG* BytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsMethod E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsProperty(PKSPROPERTY Property, ULONG PropertyLength, LPVOID PropertyData, ULONG DataLength, ULONG* BytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsProperty Entry");

    HRESULT hr = m_spIkscontrol->KsProperty(Property, PropertyLength, PropertyData, DataLength, BytesReturned);

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsProperty Exit %!HRESULT!", hr);
    return hr;
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
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamPreferredState Entry");

    HRESULT hr = E_FAIL;

    if (ppMediaType != nullptr)
    {
        ComPtr<CInPin> spiPin = GetInPin(dwStreamID);
        if (spiPin != nullptr)
        {
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamPreferredState GetInPin");

            hr = spiPin->GetInputStreamPreferredState(value, ppMediaType);
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamPreferredState spiPin->GetInputStreamPreferredState");
        }
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamPreferredState Exit %!HRESULT!", hr);
    return hr;
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
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputAvailableType GetOutPin");
            *pMediaType = nullptr;

            hr = outPin->GetOutputAvailableType(dwTypeIndex, pMediaType);
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputAvailableType GetOutputAvailableType");
        }
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputAvailableType Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputCurrentType(DWORD dwOutputStreamID, IMFMediaType** pMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputCurrentType Entry");
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputCurrentType dwOutputStreamID: %d", dwOutputStreamID);

    HRESULT hr = E_FAIL;

    EnterCriticalSection(&m_critSec);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputCurrentType EnterCriticalSection");
    if (pMediaType != NULL)
    {
        *pMediaType = nullptr;

        ComPtr<COutPin> outPin = GetOutPin(dwOutputStreamID);
        if (outPin != nullptr)
        {
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputCurrentType GetOutPin");

            hr = outPin->getMediaType(pMediaType);
        }
    }

    LeaveCriticalSection(&m_critSec);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputCurrentType LeaveCriticalSection");

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputCurrentType Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStreamAttributes(DWORD dwOutputStreamID, IMFAttributes** ppAttributes)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamAttributes Entry");
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamAttributes dwOutputStreamID: %d", dwOutputStreamID);

    HRESULT hr = S_OK;
    ComPtr<COutPin> outPin = nullptr;

    *ppAttributes = nullptr;

    outPin = GetOutPin(dwOutputStreamID);
    if (outPin != NULL)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamAttributes GetOutPin");
        hr = outPin->QueryInterface(IID_PPV_ARGS(ppAttributes));
    }

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

    // Get Ikscontrol from the source.
    hr = m_spSourceTransform.As(&m_spIkscontrol);
    if (FAILED(hr))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform m_spSourceTransform.As Failed %!HRESULT!", hr);
        spFilterUnk = nullptr;
        m_spSourceTransform = nullptr;
        return hr;
    }


    hr = m_spSourceTransform->GetStreamCount(&cInputStreams, &cOutputStreams);
    if (FAILED(hr))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform GetStreamCount Failed %!HRESULT!", hr);
        spFilterUnk = nullptr;        
        m_spSourceTransform = nullptr;
        m_spIkscontrol = nullptr;
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

                            // Try to get MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES attribute from Input PIN.
                            UINT32 uiFrameSourceType = 0;
                            if (SUCCEEDED(inPin->GetUINT32(MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES, &uiFrameSourceType)))
                            {
                                TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform inPin->GetUINT32 MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES");

                                // Set MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES attribute.
                                outPin->SetUINT32(MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES, uiFrameSourceType);
                                TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform outPin->SetUINT32");
                            }

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
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage Entry");

    HRESULT hr = E_FAIL;

    EnterCriticalSection(&m_critSec);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage EnterCriticalSection");

    switch (eMessage)
    {
    case MFT_MESSAGE_COMMAND_FLUSH:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage MFT_MESSAGE_COMMAND_FLUSH");
        hr = S_OK;
        break;
    case MFT_MESSAGE_COMMAND_DRAIN:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage MFT_MESSAGE_COMMAND_DRAIN");
        hr = S_OK;
        break;
    case MFT_MESSAGE_NOTIFY_START_OF_STREAM:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage MFT_MESSAGE_NOTIFY_START_OF_STREAM");
        hr = S_OK;
        break;
    case MFT_MESSAGE_SET_D3D_MANAGER:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage MFT_MESSAGE_SET_D3D_MANAGER");

        if (ulParam)
        {
            ComPtr<IDirect3DDeviceManager9> spD3D9Manager;
            ComPtr<IMFDXGIDeviceManager> spDXGIManager;

            // https://docs.microsoft.com/en-us/windows/win32/medfound/mft-message-set-d3d-manager
            // The MFT will query this pointer for the IDirect3DDeviceManager9 interface for Direct3D 9 and the IMFDXGIDeviceManager interface for Direct3D 11. 

            hr = ((IUnknown*)ulParam)->QueryInterface(IID_PPV_ARGS(&spD3D9Manager));
            if (SUCCEEDED(hr))
            {
                // IDirect3DDeviceManager9

                m_spDeviceManagerUnk = (IUnknown*)ulParam;
                TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage IDirect3DDeviceManager9 was passed.");
            }
            else
            {
                hr = ((IUnknown*)ulParam)->QueryInterface(IID_PPV_ARGS(&spDXGIManager));
                if (SUCCEEDED(hr))
                {
                    // IMFDXGIDeviceManager (Direct3D 11)

                    m_spDeviceManagerUnk = (IUnknown*)ulParam;
                    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage IMFDXGIDeviceManager was passed.");
                }
            }
        }
        else
        {
            m_spDeviceManagerUnk = nullptr;
            hr = S_OK;
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage IDirect3DDeviceManager9 was not passed.");
        }

        for (DWORD index = 0; index < m_basePinArrayInPins.size(); index++)
        {
            m_basePinArrayInPins[index]->SetD3DManager(m_spDeviceManagerUnk.Get());
        }
        for (DWORD index = 0; index < m_basePinArrayOutPins.size(); index++)
        {
            m_basePinArrayOutPins[index]->SetD3DManager(m_spDeviceManagerUnk.Get());
        }

        break;
    case MFT_MESSAGE_NOTIFY_BEGIN_STREAMING:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage MFT_MESSAGE_NOTIFY_BEGIN_STREAMING");

        SetStreamingState(DeviceStreamState_Run);
        hr = S_OK;
        break;
    case MFT_MESSAGE_NOTIFY_END_STREAMING:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage MFT_MESSAGE_NOTIFY_END_STREAMING");

        SetStreamingState(DeviceStreamState_Stop);
        hr = S_OK;
        break;
    case MFT_MESSAGE_NOTIFY_END_OF_STREAM:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage MFT_MESSAGE_NOTIFY_END_OF_STREAM");
        hr = S_OK;
        break;

    case MFT_MESSAGE_COMMAND_MARKER:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage MFT_MESSAGE_COMMAND_MARKER");
        break;
    case MFT_MESSAGE_COMMAND_TICK:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage MFT_MESSAGE_COMMAND_TICK");
        break;
    case MFT_MESSAGE_DROP_SAMPLES:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage MFT_MESSAGE_DROP_SAMPLES");
        break;

    }

    LeaveCriticalSection(&m_critSec);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage LeaveCriticalSection");

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, MFT_OUTPUT_DATA_BUFFER* pOutputSample, DWORD* pdwStatus)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessOutput E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetInputStreamState(DWORD dwStreamID, IMFMediaType* pMediaType, DeviceStreamState value, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetInputStreamState Entry");

    HRESULT hr = E_FAIL;
    ComPtr<CInPin> spiPin = GetInPin(dwStreamID);

    if (spiPin != nullptr)
    {
        hr = spiPin->SetInputStreamState(pMediaType, value, dwFlags);
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetInputStreamState Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetOutputStreamState(DWORD dwStreamID, IMFMediaType* pMediaType, DeviceStreamState value, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetOutputStreamState Entry");

    HRESULT hr = E_FAIL;

    EnterCriticalSection(&m_critSec);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetOutputStreamState EnterCriticalSection");

    hr = ChangeMediaTypeEx(dwStreamID, pMediaType, value);

    LeaveCriticalSection(&m_critSec);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetOutputStreamState LeaveCriticalSection");

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetOutputStreamState Exit %!HRESULT!", hr);
    return hr;
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
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::Shutdown EnterCriticalSection");

    if (m_pQueue)
    {
        hr = m_pQueue->Shutdown();
        
        m_pQueue->Release();
        m_pQueue = NULL;
    }

    LeaveCriticalSection(&m_critSec);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::Shutdown LeaveCriticalSection");

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::Shutdown Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::BeginGetEvent(IMFAsyncCallback* pCallback, IUnknown* punkState)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BeginGetEvent Entry");

    HRESULT hr = E_FAIL;

    EnterCriticalSection(&m_critSecForEventGenerator);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BeginGetEvent EnterCriticalSection m_critSecForEventGenerator");

    if (m_pQueue)
    {
        hr = m_pQueue->BeginGetEvent(pCallback, punkState);
    }

    LeaveCriticalSection(&m_critSecForEventGenerator);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BeginGetEvent LeaveCriticalSection m_critSecForEventGenerator");

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BeginGetEvent Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::EndGetEvent Entry");

    HRESULT hr = E_FAIL;

    EnterCriticalSection(&m_critSecForEventGenerator);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::EndGetEvent EnterCriticalSection m_critSecForEventGenerator");

    if (m_pQueue)
    {
        hr = m_pQueue->EndGetEvent(pResult, ppEvent);
    }

    LeaveCriticalSection(&m_critSecForEventGenerator);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::EndGetEvent LeaveCriticalSection m_critSecForEventGenerator");

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::EndGetEvent Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetEvent(DWORD dwFlags, IMFMediaEvent** ppEvent)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetEvent Entry");

    HRESULT hr = E_FAIL;

    IMFMediaEventQueue* pQueue = NULL;
    
    EnterCriticalSection(&m_critSecForEventGenerator);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetEvent EnterCriticalSection m_critSecForEventGenerator");

    if (m_pQueue)
    {
        pQueue = m_pQueue;
        pQueue->AddRef();
    }

    LeaveCriticalSection(&m_critSecForEventGenerator);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetEvent LeaveCriticalSection m_critSecForEventGenerator");

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

    EnterCriticalSection(&m_critSecForEventGenerator);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueueEvent EnterCriticalSection m_critSecForEventGenerator");
    
    if (m_pQueue)
    {
        hr = m_pQueue->QueueEventParamVar(met, guidExtendedType, hrStatus, pvValue);
    }

    LeaveCriticalSection(&m_critSecForEventGenerator);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueueEvent LeaveCriticalSection m_critSecForEventGenerator");

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

DeviceStreamState pinStateTransition[4][4] = {
    { DeviceStreamState_Stop, DeviceStreamState_Pause, DeviceStreamState_Run, DeviceStreamState_Disabled },
    { DeviceStreamState_Stop, DeviceStreamState_Pause, DeviceStreamState_Run, DeviceStreamState_Disabled },
    { DeviceStreamState_Stop, DeviceStreamState_Pause, DeviceStreamState_Run, DeviceStreamState_Disabled },
    { DeviceStreamState_Disabled, DeviceStreamState_Disabled, DeviceStreamState_Disabled, DeviceStreamState_Disabled }
};

HRESULT MyMFT::ChangeMediaTypeEx(ULONG pinId, IMFMediaType* pMediaType, DeviceStreamState newState)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx Entry");

    HRESULT hr = E_FAIL;
    ComPtr<COutPin> spoPin = GetOutPin(pinId);

    if (spoPin != nullptr)
    {
        {
            //
            // dump the media types to the logs
            //
            ComPtr<IMFMediaType> spOldMediaType;
            (VOID)spoPin->getMediaType(spOldMediaType.GetAddressOf());
            CMediaTypePrinter newType(pMediaType);
            CMediaTypePrinter oldType(spOldMediaType.Get());
            if (WPP_LEVEL_ENABLED(DMFT_GENERAL))
            {
                //TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, " Pin:%d old MT:[%s] St:%d", pinId, oldType.ToString(), newState);
                //TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, " Pin:%d new MT:[%s] St:%d", pinId, newType.ToString(), newState);
            }
        }


        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx GetOutPin");

        if (pMediaType)
        {
            GUID attrGuid;
            MF_ATTRIBUTE_TYPE pType;
            if (SUCCEEDED(pMediaType->GetItemType(attrGuid, &pType)))
            {
                string strGuid = guidToString(attrGuid);
                TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx pMediaType %s", strGuid.c_str());
            }

            ComPtr<IMFMediaType> pFullType;
            if (spoPin->IsMediaTypeSupported(pMediaType, &pFullType))
            {
                TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx spoPin->IsMediaTypeSupported");

                // Fake GetConnectedInpin method
                ComPtr<CInPin> spinPin = GetInPin(0 /* InPin Stream ID */);
                TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx GetInPin");
                
                ComPtr<IMFMediaType> pInputMediaType;
                spinPin->getMediaType(&pInputMediaType);
                TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx spinPin->getMediaType");

                DeviceStreamState oldOutPinState, oldInputStreamState, newOutStreamState, newRequestedInPinState;
                oldInputStreamState = spinPin->SetState(DeviceStreamState_Disabled);
                TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx spinPin->SetState");
                oldOutPinState = spoPin->SetState(DeviceStreamState_Disabled);
                TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx spoPin->SetState");

                // TODO
                // Flush Queue

                newOutStreamState = pinStateTransition[oldOutPinState][newState];
                newRequestedInPinState = newOutStreamState;
                
                DWORD dwFlags = 0;
                if ((newOutStreamState != oldOutPinState) /*State change*/
                    || ((pFullType.Get() != nullptr) && (pInputMediaType.Get() != nullptr) && (S_OK != (pFullType->IsEqual(pInputMediaType.Get(), &dwFlags)))) /*Media Types dont match*/
                    || ((pFullType == nullptr) || (pInputMediaType == nullptr))/*Either one of the mediatypes is null*/
                    )
                {
                    //
                    // State has change or media type has changed so we need to change the media type on the 
                    // underlying kernel pin
                    //
                    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx Changing Mediatype on the input");

                    spinPin->setPreferredMediaType(pFullType.Get());
                    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx spinPin->setPreferredMediaType");

                    spinPin->setPreferredStreamState(newRequestedInPinState);
                    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx spinPin->setPreferredStreamState");

                    SendEventToManager(METransformInputStreamStateChanged, GUID_NULL, spinPin->GetStreamId());
                    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx SendEventToManager");

                    spinPin->WaitForSetInputPinMediaChange();
                    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx spinPin->WaitForSetInputPinMediaChange");

                    spoPin->ChangeMediaTypeFromInpin(pFullType.Get(), pMediaType, newState);
                    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx spoPin->ChangeMediaTypeFromInpin");

                    SendEventToManager(MEUnknown, MEDeviceStreamCreated, spoPin->GetStreamId());
                    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx SendEventToManager");

                    spoPin->SetFirstSample(TRUE);

                    hr = S_OK;
                }
                else
                {
                    // Restore back old states as we have nothing to do
                    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx Restoring old Mediatype on the input");
                    spinPin->SetState(oldInputStreamState);
                    spoPin->SetState(oldOutPinState);

                    hr = S_OK;
                }
            }
        }

    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx Exit %!HRESULT!", hr);
    return hr;
}

HRESULT MyMFT::SendEventToManager(MediaEventType eventType, REFGUID pGuid, UINT32 context)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SendEventToManager");

    HRESULT hr = E_FAIL;
    ComPtr<IMFMediaEvent> pEvent = nullptr;

    hr = MFCreateMediaEvent(eventType, pGuid, S_OK, NULL, &pEvent);
    if (SUCCEEDED(hr))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SendEventToManager MFCreateMediaEvent");

        hr = pEvent->SetUINT32(MF_EVENT_MFT_INPUT_STREAM_ID, (ULONG)context);
        if (SUCCEEDED(hr))
        {
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SendEventToManager pEvent->SetUINT32");

            hr = QueueEvent(pEvent.Get());
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SendEventToManager QueueEvent");
        }
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SendEventToManager Exit %!HRESULT!", hr);
    return hr;
}

HRESULT MyMFT::QueueEvent(IMFMediaEvent* pEvent)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueueEvent Entry");

    HRESULT hr = E_FAIL;

    EnterCriticalSection(&m_critSecForEventGenerator);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueueEvent EnterCriticalSection m_critSecForEventGenerator");

    if (m_pQueue)
    {
        hr = m_pQueue->QueueEvent(pEvent);
    }

    LeaveCriticalSection(&m_critSecForEventGenerator);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueueEvent LeaveCriticalSection m_critSecForEventGenerator");

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueueEvent Exit %!HRESULT!", hr);
    return hr;
}

VOID MyMFT::SetStreamingState(DeviceStreamState state)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetStreamingState");

    m_StreamingState = state;
    return VOID();
}

CMediaTypePrinter::CMediaTypePrinter(IMFMediaType* _pMediaType) : pMediaType(_pMediaType), m_pBuffer(NULL)
{
}

CMediaTypePrinter::~CMediaTypePrinter()
{
    if (m_pBuffer)
    {
        delete(m_pBuffer);
    }
}
#define MEDIAPRINTER_STARTLEN  (512)
#ifndef IF_EQUAL_RETURN
#define IF_EQUAL_RETURN(param, val) if(val == param) return #val
#endif
LPCSTR GetGUIDNameConst(const GUID& guid)
{
    IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_MAJOR_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_SUBTYPE);
    IF_EQUAL_RETURN(guid, MF_MT_ALL_SAMPLES_INDEPENDENT);
    IF_EQUAL_RETURN(guid, MF_MT_FIXED_SIZE_SAMPLES);
    IF_EQUAL_RETURN(guid, MF_MT_COMPRESSED);
    IF_EQUAL_RETURN(guid, MF_MT_SAMPLE_SIZE);
    IF_EQUAL_RETURN(guid, MF_MT_WRAPPED_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_NUM_CHANNELS);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FLOAT_SAMPLES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_AVG_BYTES_PER_SECOND);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BLOCK_ALIGNMENT);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_BITS_PER_SAMPLE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_VALID_BITS_PER_SAMPLE);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_SAMPLES_PER_BLOCK);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_CHANNEL_MASK);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_FOLDDOWN_MATRIX);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKREF);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_PEAKTARGET);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGREF);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_WMADRC_AVGTARGET);
    IF_EQUAL_RETURN(guid, MF_MT_AUDIO_PREFER_WAVEFORMATEX);
    IF_EQUAL_RETURN(guid, MF_MT_AAC_PAYLOAD_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_AAC_AUDIO_PROFILE_LEVEL_INDICATION);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_SIZE);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MAX);
    IF_EQUAL_RETURN(guid, MF_MT_FRAME_RATE_RANGE_MIN);
    IF_EQUAL_RETURN(guid, MF_MT_PIXEL_ASPECT_RATIO);
    IF_EQUAL_RETURN(guid, MF_MT_DRM_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_PAD_CONTROL_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_SOURCE_CONTENT_HINT);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_CHROMA_SITING);
    IF_EQUAL_RETURN(guid, MF_MT_INTERLACE_MODE);
    IF_EQUAL_RETURN(guid, MF_MT_TRANSFER_FUNCTION);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_PRIMARIES);
    IF_EQUAL_RETURN(guid, MF_MT_CUSTOM_VIDEO_PRIMARIES);
    IF_EQUAL_RETURN(guid, MF_MT_YUV_MATRIX);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_LIGHTING);
    IF_EQUAL_RETURN(guid, MF_MT_VIDEO_NOMINAL_RANGE);
    IF_EQUAL_RETURN(guid, MF_MT_GEOMETRIC_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_MINIMUM_DISPLAY_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_APERTURE);
    IF_EQUAL_RETURN(guid, MF_MT_PAN_SCAN_ENABLED);
    IF_EQUAL_RETURN(guid, MF_MT_AVG_BITRATE);
    IF_EQUAL_RETURN(guid, MF_MT_AVG_BIT_ERROR_RATE);
    IF_EQUAL_RETURN(guid, MF_MT_MAX_KEYFRAME_SPACING);
    IF_EQUAL_RETURN(guid, MF_MT_DEFAULT_STRIDE);
    IF_EQUAL_RETURN(guid, MF_MT_PALETTE);
    IF_EQUAL_RETURN(guid, MF_MT_USER_DATA);
    IF_EQUAL_RETURN(guid, MF_MT_AM_FORMAT_TYPE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG_START_TIME_CODE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_PROFILE);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_LEVEL);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG2_FLAGS);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG_SEQUENCE_HEADER);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_0);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_0);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_SRC_PACK_1);
    IF_EQUAL_RETURN(guid, MF_MT_DV_AAUX_CTRL_PACK_1);
    IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_SRC_PACK);
    IF_EQUAL_RETURN(guid, MF_MT_DV_VAUX_CTRL_PACK);
    IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_HEADER);
    IF_EQUAL_RETURN(guid, MF_MT_ARBITRARY_FORMAT);
    IF_EQUAL_RETURN(guid, MF_MT_IMAGE_LOSS_TOLERANT);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG4_SAMPLE_DESCRIPTION);
    IF_EQUAL_RETURN(guid, MF_MT_MPEG4_CURRENT_SAMPLE_ENTRY);
    IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_4CC);
    IF_EQUAL_RETURN(guid, MF_MT_ORIGINAL_WAVE_FORMAT_TAG);

    // Media types

    IF_EQUAL_RETURN(guid, MFMediaType_Audio);
    IF_EQUAL_RETURN(guid, MFMediaType_Video);
    IF_EQUAL_RETURN(guid, MFMediaType_Protected);
    IF_EQUAL_RETURN(guid, MFMediaType_SAMI);
    IF_EQUAL_RETURN(guid, MFMediaType_Script);
    IF_EQUAL_RETURN(guid, MFMediaType_Image);
    IF_EQUAL_RETURN(guid, MFMediaType_HTML);
    IF_EQUAL_RETURN(guid, MFMediaType_Binary);
    IF_EQUAL_RETURN(guid, MFMediaType_FileTransfer);

    IF_EQUAL_RETURN(guid, MFVideoFormat_AI44); //     FCC('AI44')
    IF_EQUAL_RETURN(guid, MFVideoFormat_ARGB32); //   D3DFMT_A8R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_AYUV); //     FCC('AYUV')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DV25); //     FCC('dv25')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DV50); //     FCC('dv50')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVH1); //     FCC('dvh1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVSD); //     FCC('dvsd')
    IF_EQUAL_RETURN(guid, MFVideoFormat_DVSL); //     FCC('dvsl')
    IF_EQUAL_RETURN(guid, MFVideoFormat_H264); //     FCC('H264')
    IF_EQUAL_RETURN(guid, MFVideoFormat_I420); //     FCC('I420')
    IF_EQUAL_RETURN(guid, MFVideoFormat_IYUV); //     FCC('IYUV')
    IF_EQUAL_RETURN(guid, MFVideoFormat_M4S2); //     FCC('M4S2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MJPG);
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP43); //     FCC('MP43')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP4S); //     FCC('MP4S')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MP4V); //     FCC('MP4V')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MPG1); //     FCC('MPG1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MSS1); //     FCC('MSS1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_MSS2); //     FCC('MSS2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_NV11); //     FCC('NV11')
    IF_EQUAL_RETURN(guid, MFVideoFormat_NV12); //     FCC('NV12')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P010); //     FCC('P010')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P016); //     FCC('P016')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P210); //     FCC('P210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_P216); //     FCC('P216')
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB24); //    D3DFMT_R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB32); //    D3DFMT_X8R8G8B8 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB555); //   D3DFMT_X1R5G5B5 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB565); //   D3DFMT_R5G6B5 
    IF_EQUAL_RETURN(guid, MFVideoFormat_RGB8);
    IF_EQUAL_RETURN(guid, MFVideoFormat_UYVY); //     FCC('UYVY')
    IF_EQUAL_RETURN(guid, MFVideoFormat_v210); //     FCC('v210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_v410); //     FCC('v410')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV1); //     FCC('WMV1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV2); //     FCC('WMV2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WMV3); //     FCC('WMV3')
    IF_EQUAL_RETURN(guid, MFVideoFormat_WVC1); //     FCC('WVC1')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y210); //     FCC('Y210')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y216); //     FCC('Y216')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y410); //     FCC('Y410')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y416); //     FCC('Y416')
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y41P);
    IF_EQUAL_RETURN(guid, MFVideoFormat_Y41T);
    IF_EQUAL_RETURN(guid, MFVideoFormat_YUY2); //     FCC('YUY2')
    IF_EQUAL_RETURN(guid, MFVideoFormat_YV12); //     FCC('YV12')
    IF_EQUAL_RETURN(guid, MFVideoFormat_YVYU);

    IF_EQUAL_RETURN(guid, MFAudioFormat_PCM); //              WAVE_FORMAT_PCM 
    IF_EQUAL_RETURN(guid, MFAudioFormat_Float); //            WAVE_FORMAT_IEEE_FLOAT 
    IF_EQUAL_RETURN(guid, MFAudioFormat_DTS); //              WAVE_FORMAT_DTS 
    IF_EQUAL_RETURN(guid, MFAudioFormat_Dolby_AC3_SPDIF); //  WAVE_FORMAT_DOLBY_AC3_SPDIF 
    IF_EQUAL_RETURN(guid, MFAudioFormat_DRM); //              WAVE_FORMAT_DRM 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV8); //        WAVE_FORMAT_WMAUDIO2 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudioV9); //        WAVE_FORMAT_WMAUDIO3 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMAudio_Lossless); // WAVE_FORMAT_WMAUDIO_LOSSLESS 
    IF_EQUAL_RETURN(guid, MFAudioFormat_WMASPDIF); //         WAVE_FORMAT_WMASPDIF 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MSP1); //             WAVE_FORMAT_WMAVOICE9 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MP3); //              WAVE_FORMAT_MPEGLAYER3 
    IF_EQUAL_RETURN(guid, MFAudioFormat_MPEG); //             WAVE_FORMAT_MPEG 
    IF_EQUAL_RETURN(guid, MFAudioFormat_AAC); //              WAVE_FORMAT_MPEG_HEAAC 
    IF_EQUAL_RETURN(guid, MFAudioFormat_ADTS); //             WAVE_FORMAT_MPEG_ADTS_AAC 

    return NULL;
}
LPSTR DumpGUIDA(_In_ REFGUID guid)
{
    LPOLESTR lpszGuidString = NULL;
    char* ansiguidStr = NULL;
    if (SUCCEEDED(StringFromCLSID(guid, &lpszGuidString)))
    {
        int mbGuidLen = 0;
        mbGuidLen = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, lpszGuidString, -1, NULL, 0, NULL, NULL);
        if (mbGuidLen > 0)
        {
            ansiguidStr = new (std::nothrow) char[mbGuidLen];
            if (ansiguidStr)
            {
                WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, lpszGuidString, -1, ansiguidStr, mbGuidLen, NULL, NULL);
                CoTaskMemFree(lpszGuidString);
                ansiguidStr[mbGuidLen - 1] = 0;
            }
        }
    }
    return ansiguidStr;
}
LPSTR DumpAttribute(_In_ const MF_ATTRIBUTE_TYPE& type,
    _In_ REFPROPVARIANT var)
{
    CHAR* tempStr = NULL;
    tempStr = new (std::nothrow) CHAR[256];
    switch (type)
    {
    case MF_ATTRIBUTE_UINT32:
        if (var.vt == VT_UI4)
        {
            sprintf_s(tempStr, 256, "%u", var.ulVal);
        }
        break;
    case MF_ATTRIBUTE_UINT64:
        if (var.vt == VT_UI8)
        {
            sprintf_s(tempStr, 256, "%I64d  (high: %d low: %d)", var.uhVal.QuadPart, var.uhVal.HighPart, var.uhVal.LowPart);
        }
        break;
    case MF_ATTRIBUTE_DOUBLE:
        if (var.vt == VT_R8)
        {
            sprintf_s(tempStr, 256, "%.4f", var.dblVal);
        }
        break;
    case MF_ATTRIBUTE_GUID:
        if (var.vt == VT_CLSID)
        {
            return DumpGUIDA(*var.puuid);
        }
        break;
    case MF_ATTRIBUTE_STRING:
        if (var.vt == VT_LPWSTR)
        {
            sprintf_s(tempStr, 256, "%S", var.pwszVal);
        }
        break;
    case MF_ATTRIBUTE_IUNKNOWN:
        break;
    default:
        printf("(Unknown Attribute Type = %d) ", type);
        break;
    }
    return tempStr;
}
#define checkAdjustBufferCap(a,len){\
    char* tStore = NULL; \
if (a && strlen(a) > ((len * 7) / 10)){\
    tStore = a; \
    len *= 2; \
    a = new (std::nothrow) char[len]; \
if (!a){\
goto done;}\
    a[0] = 0; \
    strcat_s(a, len, tStore); \
    delete(tStore); }\
}
STDMETHODIMP_(PCHAR __stdcall) CMediaTypePrinter::ToCompleteString()
{
    HRESULT             hr = S_OK;
    UINT32              attrCount = 0;
    GUID                attrGuid = { 0 };
    char* tempStore = nullptr;
    PROPVARIANT var;
    LPSTR pTempBaseStr;
    MF_ATTRIBUTE_TYPE   pType;

    if (pMediaType && !m_pBuffer)
    {
        pMediaType->GetCount(&attrCount);
        buffLen = MEDIAPRINTER_STARTLEN;
        m_pBuffer = new char[buffLen];
        if (m_pBuffer != NULL)
        {
            m_pBuffer[0] = 0;
            for (UINT32 ulIndex = 0; ulIndex < attrCount; ulIndex++)
            {
                PropVariantInit(&var);
                checkAdjustBufferCap(m_pBuffer, buffLen);
                pMediaType->GetItemByIndex(ulIndex, &attrGuid, &var);
                pMediaType->GetItemType(attrGuid, &pType);
                if (ulIndex > 0)
                    strcat_s(m_pBuffer, MEDIAPRINTER_STARTLEN, " : ");
                strcat_s(m_pBuffer, buffLen, GetGUIDNameConst(attrGuid));
                strcat_s(m_pBuffer, buffLen, "=");
                pTempBaseStr = DumpAttribute(pType, var);
                strcat_s(m_pBuffer, buffLen, pTempBaseStr);
                delete(pTempBaseStr);
                PropVariantClear(&var);
            }
        }

        if (tempStore)
        {
            delete(tempStore);
        }
    }
done:
    return m_pBuffer;
}

STDMETHODIMP_(PCHAR __stdcall) CMediaTypePrinter::ToString()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CMediaTypePrinter::ToString Entry");

    //
    //Following are the important ones of Mediatype attributes
    //

    HRESULT     hr = S_OK;
    PROPVARIANT var;
    LPSTR pTempBaseStr;
    MF_ATTRIBUTE_TYPE   pType;
    GUID                attrGuid;
    GUID impGuids[] = {
        MF_MT_SUBTYPE,
        MF_MT_FRAME_SIZE,
        MF_MT_SAMPLE_SIZE,
        MF_MT_FRAME_RATE,
        MF_MT_DEFAULT_STRIDE,
        MF_XVP_DISABLE_FRC
    };

    if (pMediaType && !m_pBuffer)
    {
        buffLen = MEDIAPRINTER_STARTLEN;
        m_pBuffer = new (std::nothrow) char[buffLen];
        if (m_pBuffer != NULL)
        {
            m_pBuffer[0] = 0;
            for (UINT32 ulIndex = 0; ulIndex < ARRAYSIZE(impGuids); ulIndex++)
            {
                PropVariantInit(&var);
                checkAdjustBufferCap(m_pBuffer, buffLen);
                attrGuid = impGuids[ulIndex];
                pMediaType->GetItemType(attrGuid, &pType);
                pMediaType->GetItem(attrGuid, &var);
                if (ulIndex > 0)
                    strcat_s(m_pBuffer, MEDIAPRINTER_STARTLEN, " : ");
                strcat_s(m_pBuffer, buffLen, GetGUIDNameConst(attrGuid));
                strcat_s(m_pBuffer, buffLen, "=");
                pTempBaseStr = DumpAttribute(pType, var);
                strcat_s(m_pBuffer, buffLen, pTempBaseStr);
                delete(pTempBaseStr);
                PropVariantClear(&var);
            }
        }
    }
done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CMediaTypePrinter::ToString m_pBuffer: %s", m_pBuffer);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CMediaTypePrinter::ToString Exit");
    return m_pBuffer;

}
