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

DeviceStreamState pinStateTransition[4][4] = {
    { DeviceStreamState_Stop, DeviceStreamState_Pause, DeviceStreamState_Run, DeviceStreamState_Disabled },
    { DeviceStreamState_Stop, DeviceStreamState_Pause, DeviceStreamState_Run, DeviceStreamState_Disabled },
    { DeviceStreamState_Stop, DeviceStreamState_Pause, DeviceStreamState_Run, DeviceStreamState_Disabled },
    { DeviceStreamState_Disabled, DeviceStreamState_Disabled, DeviceStreamState_Disabled, DeviceStreamState_Disabled }
};

LPSTR DumpGUIDA(_In_ REFGUID guid);
void printMessageEvent(MFT_MESSAGE_TYPE msg);

MyMFT::MyMFT()
{
    m_pQueue = NULL;

    MFCreateEventQueue(&m_pQueue);
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
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueryInterface IMFTransform");
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

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetWorkQueueEx(DWORD dwWorkQueueId, LONG lWorkItemBasePriority)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetWorkQueueEx -----");

    CAutoLock   lock(m_critSec);
    //
    // Cache the WorkQueuId and WorkItemBasePriority. This is called once soon after the device MFT is initialized
    //
    m_dwWorkQueueId = dwWorkQueueId;
    m_lWorkQueuePriority = lWorkItemBasePriority;
    // Set it on the pins
    for (DWORD dwIndex = 0; dwIndex < (DWORD)m_InPins.size(); dwIndex++)
    {
        m_InPins[dwIndex]->SetWorkQueue(dwWorkQueueId);
    }
    for (DWORD dwIndex = 0; dwIndex < (DWORD)m_OutPins.size(); dwIndex++)
    {
        m_OutPins[dwIndex]->SetWorkQueue(dwWorkQueueId);
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetWorkQueueEx Exit -----");
    return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::UnregisterThreads()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::UnregisterThreads E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsEvent(PKSEVENT pEvent, ULONG ulEventLength, LPVOID pEventData, ULONG ulDataLength, ULONG* pBytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsEvent E_NOTIMPL -----");

    UNREFERENCED_PARAMETER(pBytesReturned);
    UNREFERENCED_PARAMETER(ulDataLength);
    UNREFERENCED_PARAMETER(pEventData);
    UNREFERENCED_PARAMETER(pEvent);
    UNREFERENCED_PARAMETER(ulEventLength);
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsMethod(PKSMETHOD pMethod, ULONG ulMethodLength, LPVOID pMethodData, ULONG ulDataLength, ULONG* pBytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsMethod E_NOTIMPL -----");

    UNREFERENCED_PARAMETER(pBytesReturned);
    UNREFERENCED_PARAMETER(ulDataLength);
    UNREFERENCED_PARAMETER(pMethodData);
    UNREFERENCED_PARAMETER(pMethod);
    UNREFERENCED_PARAMETER(ulMethodLength);
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::KsProperty(PKSPROPERTY pProperty, ULONG ulPropertyLength, LPVOID pvPropertyData, ULONG ulDataLength, ULONG* pulBytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::KsProperty -----");

    HRESULT hr = S_OK;
    UNREFERENCED_PARAMETER(pulBytesReturned);
    DMFTCHECKNULL_GOTO(pProperty, done, E_INVALIDARG);
    DMFTCHECKNULL_GOTO(pulBytesReturned, done, E_INVALIDARG);

    //
    // Enable Warm Start on All filters for the sample. Please comment out this
    // section if this is not needed
    //
    if (IsEqualCLSID(pProperty->Set, KSPROPERTYSETID_ExtendedCameraControl)
        && (pProperty->Id == KSPROPERTY_CAMERACONTROL_EXTENDED_WARMSTART))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! Warm Start Control %d Passed ", pProperty->Id);
    }

    if (IsEqualCLSID(pProperty->Set, KSPROPERTYSETID_ExtendedCameraControl))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! Extended Control %d Passed ", pProperty->Id);
    }
    else if ((IsEqualCLSID(pProperty->Set, PROPSETID_VIDCAP_VIDEOCONTROL)) && (pProperty->Id == KSPROPERTY_VIDEOCONTROL_MODE))
    {
        // A function illustrating how we can capture and service photos from the device MFT. This block shows how we can
        // intercept Photo triggers going down to the pipeline

        if (sizeof(KSPROPERTY_VIDEOCONTROL_MODE_S) == ulDataLength)
        {
            PKSPROPERTY_VIDEOCONTROL_MODE_S VideoControl = (PKSPROPERTY_VIDEOCONTROL_MODE_S)pvPropertyData;
            /*
            m_PhotoModeIsPhotoSequence = false;
            if (VideoControl->Mode == KS_VideoControlFlag_StartPhotoSequenceCapture)
            {
                DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! Starting PhotoSequence Trigger");
                m_PhotoModeIsPhotoSequence = true;
            }
            else if (VideoControl->Mode == KS_VideoControlFlag_StopPhotoSequenceCapture)
            {
                DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! Stopping PhotoSequence Trigger");
                m_PhotoModeIsPhotoSequence = false;
            }
            else
            {
                DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! Take Single Photo Trigger");
            }
            */
        }
    }
    DMFTCHECKHR_GOTO(m_spIkscontrol->KsProperty(pProperty,
        ulPropertyLength,
        pvPropertyData,
        ulDataLength,
        pulBytesReturned), done);
done:
    LPSTR guidStr = DumpGUIDA(pProperty->Set);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! g:%s p:%d exiting %x = %!HRESULT! -----", guidStr, pProperty->Id, hr, hr);
    delete(guidStr);
    return hr;
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

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamAttributes(DWORD dwInputStreamID, IMFAttributes** ppAttributes)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamAttributes -----");

    HRESULT hr = S_OK;
    ComPtr<CInPin> spIPin;

    DMFTCHECKNULL_GOTO(ppAttributes, done, E_INVALIDARG);
    *ppAttributes = nullptr;

    spIPin = GetInPin(dwInputStreamID);

    DMFTCHECKNULL_GOTO(spIPin, done, E_INVALIDARG);

    hr = spIPin->getPinAttributes(ppAttributes);

done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamPreferredState(DWORD dwStreamID, DeviceStreamState* value, IMFMediaType** ppMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamPreferredState Entry -----");

    HRESULT hr = S_OK;
    ComPtr<CInPin> spiPin = GetInPin(dwStreamID);
    DMFTCHECKNULL_GOTO(ppMediaType, done, E_INVALIDARG);
    DMFTCHECKNULL_GOTO(spiPin, done, MF_E_INVALIDSTREAMNUMBER);
    hr = spiPin->GetInputStreamPreferredState(value, ppMediaType);
done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetInputStreamState(DWORD, DeviceStreamState*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInputStreamState E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputAvailableType(DWORD dwOutputStreamID, DWORD dwTypeIndex, IMFMediaType** ppMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputAvailableType -----");

    HRESULT hr = S_OK;
    CAutoLock Lock(m_critSec);

    ComPtr<COutPin> spoPin = GetOutPin(dwOutputStreamID);

    DMFTCHECKNULL_GOTO(spoPin.Get(), done, MF_E_INVALIDSTREAMNUMBER);
    DMFTCHECKNULL_GOTO(ppMediaType, done, E_INVALIDARG);

    *ppMediaType = nullptr;

    hr = spoPin->GetOutputAvailableType(dwTypeIndex, ppMediaType);

    if (FAILED(hr))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! Pin: %d Index: %d exiting  %!HRESULT!", dwOutputStreamID, dwTypeIndex, hr);
    }

done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputCurrentType(DWORD dwOutputStreamID, IMFMediaType** ppMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputCurrentType Entry -----");

    HRESULT hr = S_OK;
    ComPtr<COutPin> spoPin;
    CAutoLock lock(m_critSec);

    DMFTCHECKNULL_GOTO(ppMediaType, done, E_INVALIDARG);

    *ppMediaType = nullptr;

    spoPin = GetOutPin(dwOutputStreamID);

    DMFTCHECKNULL_GOTO(spoPin, done, MF_E_INVALIDSTREAMNUMBER);

    DMFTCHECKHR_GOTO(spoPin->getMediaType(ppMediaType), done);

    DMFTCHECKNULL_GOTO(*ppMediaType, done, MF_E_TRANSFORM_TYPE_NOT_SET);

done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStreamAttributes(DWORD dwOutputStreamID, IMFAttributes** ppAttributes)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamAttributes -----");

    HRESULT hr = S_OK;
    ComPtr<COutPin> spoPin;

    DMFTCHECKNULL_GOTO(ppAttributes, done, E_INVALIDARG);

    *ppAttributes = nullptr;

    spoPin = GetOutPin(dwOutputStreamID);

    DMFTCHECKNULL_GOTO(spoPin, done, E_INVALIDARG);

    DMFTCHECKHR_GOTO(spoPin->getPinAttributes(ppAttributes), done);
done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetOutputStreamState(DWORD, DeviceStreamState*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutputStreamState E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetStreamCount(DWORD* pdwInputStreams, DWORD* pdwOutputStreams)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamCount -----");

    HRESULT hr = S_OK;
    CAutoLock   lock(m_critSec);
    DMFTCHECKNULL_GOTO(pdwInputStreams, done, E_INVALIDARG);
    DMFTCHECKNULL_GOTO(pdwOutputStreams, done, E_INVALIDARG);
    *pdwInputStreams = m_InputPinCount;
    *pdwOutputStreams = m_OutputPinCount;

done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::GetStreamIDs(DWORD dwInputIDArraySize, DWORD* pdwInputIDs, DWORD  dwOutputIDArraySize, DWORD* pdwOutputIDs)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetStreamIDs -----");

    HRESULT hr = S_OK;
    CAutoLock   lock(m_critSec);
    if ((dwInputIDArraySize < m_InputPinCount) && (dwOutputIDArraySize < m_OutputPinCount))
    {
        hr = MF_E_BUFFERTOOSMALL;
        goto done;
    }

    if (dwInputIDArraySize)
    {
        DMFTCHECKNULL_GOTO(pdwInputIDs, done, E_POINTER);
        for (DWORD dwIndex = 0; dwIndex < ((dwInputIDArraySize > m_InputPinCount) ? m_InputPinCount :
            dwInputIDArraySize); dwIndex++)
        {
            pdwInputIDs[dwIndex] = (m_InPins[dwIndex])->streamId();
        }
    }

    if (dwOutputIDArraySize)
    {
        DMFTCHECKNULL_GOTO(pdwOutputIDs, done, E_POINTER);
        for (DWORD dwIndex = 0; dwIndex < ((dwOutputIDArraySize > m_OutputPinCount) ? m_OutputPinCount :
            dwOutputIDArraySize); dwIndex++)
        {
            pdwOutputIDs[dwIndex] = (m_OutPins[dwIndex])->streamId();
        }
    }
done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::InitializeTransform(IMFAttributes* pAttributes)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::InitializeTransform -----");

    HRESULT                 hr = S_OK;
    ComPtr<IUnknown>        spFilterUnk = nullptr;
    DWORD* pcInputStreams = NULL, * pcOutputStreams = NULL;
    DWORD                   inputStreams = 0;
    DWORD                   outputStreams = 0;
    GUID* outGuids = NULL;
    GUID                    streamCategory = GUID_NULL;
    ULONG                   ulOutPinIndex = 0;
    UINT32                  uiSymLinkLen = 0;
    CPinCreationFactory* pPinFactory = new (std::nothrow) CPinCreationFactory(this);
    DMFTCHECKNULL_GOTO(pAttributes, done, E_INVALIDARG);
    //
    // The attribute passed with MF_DEVICEMFT_CONNECTED_FILTER_KSCONTROL is the source transform. This generally represents a filter
    // This needs to be stored so that we know the device properties. We cache it. We query for the IKSControl which is used to send
    // controls to the driver.
    //
    DMFTCHECKHR_GOTO(pAttributes->GetUnknown(MF_DEVICEMFT_CONNECTED_FILTER_KSCONTROL, IID_PPV_ARGS(&spFilterUnk)), done);

    if (SUCCEEDED(pAttributes->GetStringLength(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, &uiSymLinkLen))) // Not available prior to RS5
    {
        m_SymbolicLink = new (std::nothrow) WCHAR[++uiSymLinkLen];
        DMFTCHECKNULL_GOTO(m_SymbolicLink, done, E_OUTOFMEMORY);
        DMFTCHECKHR_GOTO(pAttributes->GetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, m_SymbolicLink, uiSymLinkLen, &uiSymLinkLen), done);
    }

    DMFTCHECKHR_GOTO(spFilterUnk.As(&m_spSourceTransform), done);

    DMFTCHECKHR_GOTO(m_spSourceTransform.As(&m_spIkscontrol), done);

    //DMFTCHECKHR_GOTO(m_spSourceTransform->MFTGetStreamCount(&inputStreams, &outputStreams), done);
    DMFTCHECKHR_GOTO(m_spSourceTransform->GetStreamCount(&inputStreams, &outputStreams), done);

    spFilterUnk = nullptr;

    //
    //The number of input pins created by the device transform should match the pins exposed by
    //the source transform i.e. outputStreams from SourceTransform or DevProxy = Input pins of the Device MFT
    //

    if (inputStreams > 0 || outputStreams > 0)
    {
        pcInputStreams = new (std::nothrow) DWORD[inputStreams];
        DMFTCHECKNULL_GOTO(pcInputStreams, done, E_OUTOFMEMORY);

        pcOutputStreams = new (std::nothrow) DWORD[outputStreams];
        DMFTCHECKNULL_GOTO(pcOutputStreams, done, E_OUTOFMEMORY);

        //DMFTCHECKHR_GOTO(m_spSourceTransform->MFTGetStreamIDs(inputStreams, pcInputStreams, outputStreams, pcOutputStreams), done);
        DMFTCHECKHR_GOTO(m_spSourceTransform->GetStreamIDs(inputStreams, pcInputStreams, outputStreams, pcOutputStreams), done);

        //
        // Output pins from DevProxy = Input pins of device MFT.. We are the first transform in the pipeline before MFT0
        //

        for (ULONG ulIndex = 0; ulIndex < outputStreams; ulIndex++)
        {
            ComPtr<IMFAttributes>   pInAttributes = nullptr;
            BOOL                    bCustom = FALSE;
            ComPtr<CInPin>          spInPin;

            DMFTCHECKHR_GOTO(pPinFactory->CreatePin(
                pcOutputStreams[ulIndex], /*Input Pin ID as advertised by the pipeline*/
                0, /*This is not needed for Input Pin*/
                CPinCreationFactory::DMFT_PIN_INPUT, /*Input Pin*/
                (CBasePin**)spInPin.GetAddressOf(),
                bCustom), done);
            hr = ExceptionBoundary([&]()
                {
                    m_InPins.push_back(spInPin.Get());
                });
            DMFTCHECKHR_GOTO(hr, done);
            DMFTCHECKHR_GOTO(spInPin->Init(m_spSourceTransform.Get()), done);
            spInPin.Detach();
        }

        //
        // Create one on one mapping
        //
        for (ULONG ulIndex = 0; ulIndex < m_InPins.size(); ulIndex++)
        {

            ComPtr<COutPin> spoPin;
            BOOL     bCustom = FALSE;
            ComPtr<CInPin> spiPin = (CInPin*)m_InPins[ulIndex];

            if (spiPin.Get())
            {
                DMFTCHECKHR_GOTO(pPinFactory->CreatePin(spiPin->streamId(), /*Input Pin connected to the Output Pin*/
                    ulOutPinIndex, /*Output pin Id*/
                    CPinCreationFactory::DMFT_PIN_OUTPUT, /*Output pin */
                    (CBasePin**)spoPin.ReleaseAndGetAddressOf(),
                    bCustom), done);
                hr = BridgeInputPinOutputPin(spiPin.Get(), spoPin.Get());
                if (SUCCEEDED(hr))
                {
                    DMFTCHECKHR_GOTO(ExceptionBoundary([&]()
                        {
                            m_OutPins.push_back(spoPin.Get());
                        }), done);
                    spoPin.Detach();
                    ulOutPinIndex++;
                    hr = S_OK;
                }
                if (hr == MF_E_INVALID_STREAM_DATA)
                {
                    // Skip the pin which doesn't have any mediatypes exposed
                    hr = S_OK;
                }
                DMFTCHECKHR_GOTO(hr, done);
            }
        }

    }

    m_InputPinCount = ULONG(m_InPins.size());
    m_OutputPinCount = ULONG(m_OutPins.size());

done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);

    if (pcInputStreams)
    {
        delete[](pcInputStreams);
    }
    if (pcOutputStreams)
    {
        delete[](pcOutputStreams);
    }
    if (outGuids)
    {
        delete[](outGuids);
    }
    SAFE_DELETE(pPinFactory);
    if (FAILED(hr))
    {
        //Release the pins and the resources acquired
        for (ULONG ulIndex = 0, ulSize = (ULONG)m_InPins.size(); ulIndex < ulSize; ulIndex++)
        {
            SAFERELEASE(m_InPins[ulIndex]);
        }
        m_InPins.clear();
        for (ULONG ulIndex = 0, ulSize = (ULONG)m_OutPins.size(); ulIndex < ulSize; ulIndex++)
        {
            SAFERELEASE(m_OutPins[ulIndex]);
        }
        m_OutPins.clear();
        //
        // Simply clear the custom pins since the input pins must have deleted the pin
        //
        m_spSourceTransform = nullptr;
        m_spIkscontrol = nullptr;
    }
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessEvent(DWORD, IMFMediaEvent*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessEvent E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessInput(DWORD, IMFSample*, DWORD)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessInput E_NOTIMPL-----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessMessage(MFT_MESSAGE_TYPE eMessage, ULONG_PTR ulParam)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessMessage -----");

    UNREFERENCED_PARAMETER(ulParam);
    printMessageEvent(eMessage);
    return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::ProcessOutput(DWORD, DWORD, MFT_OUTPUT_DATA_BUFFER*, DWORD*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ProcessOutput E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetInputStreamState(DWORD dwStreamID, IMFMediaType* pMediaType, DeviceStreamState value, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetInputStreamState Entry -----");

    HRESULT hr = S_OK;
    ComPtr<CInPin> spiPin = GetInPin(dwStreamID);
    DMFTCHECKNULL_GOTO(spiPin, done, MF_E_INVALIDSTREAMNUMBER);

    DMFTCHECKHR_GOTO(spiPin->SetInputStreamState(pMediaType, value, dwFlags), done);

done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::SetOutputStreamState(DWORD dwStreamID, IMFMediaType* pMediaType, DeviceStreamState state, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SetOutputStreamState -----");

    HRESULT hr = S_OK;
    UNREFERENCED_PARAMETER(dwFlags);
    CAutoLock Lock(m_critSec);

    DMFTCHECKHR_GOTO(ChangeMediaTypeEx(dwStreamID, pMediaType, state), done);

done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;
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

STDMETHODIMP_(HRESULT __stdcall) MyMFT::BeginGetEvent(IMFAsyncCallback* pCallback, IUnknown* pState)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BeginGetEvent -----");

    HRESULT hr = S_OK;
    m_critSecForEvent.Lock();

    //hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = m_pQueue->BeginGetEvent(pCallback, pState);
    }

    m_critSecForEvent.Unlock();

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) MyMFT::EndGetEvent(IMFAsyncResult* pResult, IMFMediaEvent** ppEvent)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::EndGetEvent -----");

    HRESULT hr = S_OK;
    m_critSecForEvent.Lock();

    //hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        hr = m_pQueue->EndGetEvent(pResult, ppEvent);
    }

    m_critSecForEvent.Unlock();

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;
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

STDMETHODIMP_(HRESULT __stdcall) MyMFT::QueueEvent(IMFMediaEvent* pEvent)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::QueueEvent 2 Entry -----");

    HRESULT hr = S_OK;
    m_critSecForEvent.Lock();

    //hr = CheckShutdown();

    if (SUCCEEDED(hr))
    {
        if (m_pQueue)
        {
            hr = m_pQueue->QueueEvent(pEvent);
        }
    }

    m_critSecForEvent.Unlock();

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;
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

HRESULT MyMFT::SendEventToManager(MediaEventType eventType, REFGUID pGuid, UINT32 context)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::SendEventToManager Entry");

    HRESULT hr = S_OK;
    ComPtr<IMFMediaEvent>  pEvent = nullptr;

    DMFTCHECKHR_GOTO(MFCreateMediaEvent(eventType, pGuid, S_OK, NULL, &pEvent), done);
    DMFTCHECKHR_GOTO(pEvent->SetUINT32(MF_EVENT_MFT_INPUT_STREAM_ID, (ULONG)context), done);
    DMFTCHECKHR_GOTO(QueueEvent(pEvent.Get()), done);
done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);

    return hr;
}

HRESULT MyMFT::BridgeInputPinOutputPin(CInPin* piPin, COutPin* poPin)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::BridgeInputPinOutputPin Entry");

    HRESULT hr = S_OK;
    ULONG   ulIndex = 0;
    ULONG   ulAddedMediaTypeCount = 0;
    ComPtr<IMFMediaType> spMediaType;

    DMFTCHECKNULL_GOTO(piPin, done, E_INVALIDARG);
    DMFTCHECKNULL_GOTO(poPin, done, E_INVALIDARG);
    //
    // Copy over the media types from input pin to output pin. Since there is no
    // decoder support, only the uncompressed media types are inserted. Please make
    // sure any pin advertised supports at least one media type. The pipeline doesn't
    // like pins with no media types
    //
    while (SUCCEEDED(hr = piPin->GetMediaTypeAt(ulIndex++, spMediaType.ReleaseAndGetAddressOf())))
    {
        GUID subType = GUID_NULL;
        DMFTCHECKHR_GOTO(spMediaType->GetGUID(MF_MT_SUBTYPE, &subType), done);
        {
            DMFTCHECKHR_GOTO(hr = poPin->AddMediaType(NULL, spMediaType.Get()), done);
            if (hr == S_OK)
            {
                ulAddedMediaTypeCount++;
            }
        }
    }
    if (ulAddedMediaTypeCount == 0)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! Make Sure Pin %d has one media type exposed ", piPin->streamId());
        DMFTCHECKHR_GOTO(MF_E_INVALID_STREAM_DATA, done);
    }
    //
    //Add the Input Pin to the output Pin
    //
    DMFTCHECKHR_GOTO(poPin->AddPin(piPin->streamId()), done);
    hr = ExceptionBoundary([&]() {
        //
        // Add the output pin to the input pin. 
        // Create the pin map. So that we know which pin input pin is connected to which output pin
        //
        piPin->ConnectPin(poPin);
        m_outputPinMap.insert(std::pair< int, int >(poPin->streamId(), piPin->streamId()));
        });

done:
    //
    //Failed adding media types
    //
    if (FAILED(hr))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;

}

CInPin* MyMFT::GetInPin(DWORD dwStreamId)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetInPin Entry");

    CInPin* inPin = NULL;
    for (DWORD dwIndex = 0, dwSize = (DWORD)m_InPins.size(); dwIndex < dwSize; dwIndex++)
    {
        inPin = (CInPin*)m_InPins[dwIndex];
        if (dwStreamId == inPin->streamId())
        {
            break;
        }
        inPin = NULL;
    }
    return inPin;
}

COutPin* MyMFT::GetOutPin(DWORD dwStreamId)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetOutPin Entry");

    COutPin* outPin = NULL;
    for (DWORD dwIndex = 0, dwSize = (DWORD)m_OutPins.size(); dwIndex < dwSize; dwIndex++)
    {
        outPin = (COutPin*)m_OutPins[dwIndex];

        if (dwStreamId == outPin->streamId())
        {
            break;
        }

        outPin = NULL;
    }

    return outPin;
}

HRESULT MyMFT::GetConnectedInpin(ULONG ulOutpin, ULONG& ulInPin)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::GetConnectedInpin Entry");

    HRESULT hr = S_OK;
    map<int, int>::iterator it = m_outputPinMap.find(ulOutpin);
    if (it != m_outputPinMap.end())
    {
        ulInPin = it->second;
    }
    else
    {
        hr = MF_E_INVALIDSTREAMNUMBER;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;
}

HRESULT MyMFT::ChangeMediaTypeEx(ULONG pinId, IMFMediaType* pMediaType, DeviceStreamState reqState)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "MyMFT::ChangeMediaTypeEx Entry");

    HRESULT hr = S_OK;
    ComPtr<COutPin> spoPin = GetOutPin(pinId);
    ComPtr<CInPin> spinPin;
    DeviceStreamState       oldOutPinState, oldInputStreamState, newOutStreamState, newRequestedInPinState;
    ComPtr<IMFMediaType>    pFullType, pInputMediaType;
    ULONG                   ulInPinId = 0;
    DWORD                   dwFlags = 0;


    DMFTCHECKNULL_GOTO(spoPin, done, E_INVALIDARG);
    {
        //
        // dump the media types to the logs
        //
        ComPtr<IMFMediaType> spOldMediaType;
        (VOID)spoPin->getMediaType(spOldMediaType.GetAddressOf());
        CMediaTypePrinter newType(pMediaType);
        CMediaTypePrinter oldType(spOldMediaType.Get());
        //if (WPP_LEVEL_ENABLED(DMFT_GENERAL))
        //{
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, " Pin:%d old MT:[%s] St:%d", pinId, oldType.ToString(), reqState);
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, " Pin:%d new MT:[%s] St:%d", pinId, newType.ToString(), reqState);
        //}
    }

    if (pMediaType)
    {
        if (!spoPin->IsMediaTypeSupported(pMediaType, &pFullType))
        {
            DMFTCHECKHR_GOTO(MF_E_INVALIDMEDIATYPE, done);
        }
    }

    DMFTCHECKHR_GOTO(GetConnectedInpin(pinId, ulInPinId), done);
    spinPin = GetInPin(ulInPinId); // Get the input pin

    (VOID)spinPin->getMediaType(&pInputMediaType);
    oldInputStreamState = spinPin->SetState(DeviceStreamState_Disabled); // Disable input pin
    oldOutPinState = spoPin->SetState(DeviceStreamState_Disabled);  // Disable output pin
    newOutStreamState = pinStateTransition[oldOutPinState][reqState];  // New state needed  

    // The Old input and the output pin states should be the same
    newRequestedInPinState = newOutStreamState;

    if ((newOutStreamState != oldOutPinState) /*State change*/
        || ((pFullType.Get() != nullptr) && (pInputMediaType.Get() != nullptr) && (S_OK != (pFullType->IsEqual(pInputMediaType.Get(), &dwFlags)))) /*Media Types dont match*/
        || ((pFullType == nullptr) || (pInputMediaType == nullptr))/*Either one of the mediatypes is null*/
        )
    {
        //
        // State has change or media type has changed so we need to change the media type on the 
        // underlying kernel pin
        //
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "Changing Mediatype on the input ");
        spinPin->setPreferredMediaType(pFullType.Get());
        spinPin->setPreferredStreamState(newRequestedInPinState);
        // Let the pipline know that the input needs to be changed. 
        SendEventToManager(METransformInputStreamStateChanged, GUID_NULL, spinPin->streamId());
        //
        //  The media type will be set on the input pin by the time we return from the wait
        //          
        DMFTCHECKHR_GOTO(spinPin->WaitForSetInputPinMediaChange(), done);
        // Change the media type on the output..
        DMFTCHECKHR_GOTO(spoPin->ChangeMediaTypeFromInpin(pFullType.Get(), pMediaType, reqState), done);
        //
        // Notify the pipeline that the output stream media type has changed
        //
        DMFTCHECKHR_GOTO(SendEventToManager(MEUnknown, MEDeviceStreamCreated, spoPin->streamId()), done);
        spoPin->SetFirstSample(TRUE);
    }
    else
    {
        // Restore back old states as we have nothing to do
        spinPin->SetState(oldInputStreamState);
        spoPin->SetState(oldOutPinState);
    }

done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;
}

HRESULT CPinCreationFactory::CreatePin(ULONG ulInputStreamId, ULONG ulOutStreamId, type_pin type, CBasePin** ppPin, BOOL& isCustom)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CPinCreationFactory::CreatePin Entry");

    HRESULT hr = S_OK;
    ComPtr<IMFAttributes> spAttributes;
    GUID    streamCategory = GUID_NULL;
    if (!ppPin)
    {
        hr = E_INVALIDARG;
        goto done;
    }

    *ppPin = nullptr;
    isCustom = FALSE;

    hr = m_spDeviceTransform->Parent()->GetOutputStreamAttributes(ulInputStreamId, &spAttributes);
    if (FAILED(hr))
    {
        goto done;
    }

    if (type == DMFT_PIN_INPUT)
    {
        ComPtr<CInPin>  spInPin;
        spInPin = new (std::nothrow) CInPin(spAttributes.Get(), ulInputStreamId, m_spDeviceTransform.Get());
        if (!spInPin.Get())
        {
            hr = E_OUTOFMEMORY;
            goto done;
        }

        *ppPin = spInPin.Detach();

    }
    else if (type == DMFT_PIN_OUTPUT)
    {
        ComPtr<COutPin> spOutPin;
        ComPtr<IKsControl>  spKscontrol;
        ComPtr<CInPin>      spInPin;
        GUID                pinGuid = GUID_NULL;
        UINT32              uiFrameSourceType = 0;

        spInPin = static_cast<CInPin*>(m_spDeviceTransform->GetInPin(ulInputStreamId));              // Get the Input Pin connected to the Output pin
        DMFTCHECKNULL_GOTO(spInPin.Get(), done, E_INVALIDARG);
        DMFTCHECKHR_GOTO(spInPin.As(&spKscontrol), done);   // Grab the IKSControl off the input pin
        DMFTCHECKHR_GOTO(spInPin->GetGUID(MF_DEVICESTREAM_STREAM_CATEGORY, &pinGuid), done);         // Get the Stream Category. Advertise on the output pin

        spOutPin = new (std::nothrow) COutPin(ulOutStreamId, m_spDeviceTransform.Get(), spKscontrol.Get(), MFSampleAllocatorUsage_DoesNotAllocate);         // Create the output pin
        DMFTCHECKNULL_GOTO(spOutPin.Get(), done, E_OUTOFMEMORY);
        DMFTCHECKHR_GOTO(spOutPin->SetGUID(MF_DEVICESTREAM_STREAM_CATEGORY, pinGuid), done);         // Advertise the Stream category to the Pipeline
        DMFTCHECKHR_GOTO(spOutPin->SetUINT32(MF_DEVICESTREAM_STREAM_ID, ulOutStreamId), done);       // Advertise the stream Id to the Pipeline
        //
        // @@@@ README
        // Note H264 pins are tagged MFFrameSourceTypes_Custom. Since we are decoding H264 if we enable decoding,
        // lets change it to color, because we are producing an uncompressed format type, hence change it to 
        //    MFFrameSourceTypes_Color, MFFrameSourceTypes_Infrared or MFFrameSourceTypes_Depth
        //
        if (SUCCEEDED(spInPin->GetUINT32(MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES, &uiFrameSourceType)))
        {
            DMFTCHECKHR_GOTO(spOutPin->SetUINT32(MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES, uiFrameSourceType), done);   // Copy over the Frame Source Type.
        }

        *ppPin = spOutPin.Detach();
    }
    else
    {
        DMFTCHECKHR_GOTO(E_INVALIDARG, done);
    }

done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;

}

CMediaTypePrinter::CMediaTypePrinter(
    _In_ IMFMediaType* _pMediaType)
    : pMediaType(_pMediaType),
    m_pBuffer(NULL)
{
}

CMediaTypePrinter::~CMediaTypePrinter()
{
    if (m_pBuffer)
    {
        delete(m_pBuffer);
    }
}
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

/*++
Description:
Rudimentary function to print the complete Media type
--*/
PCHAR CMediaTypePrinter::ToCompleteString()
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
        DMFTCHECKHR_GOTO(pMediaType->GetCount(&attrCount), done);
        buffLen = MEDIAPRINTER_STARTLEN;
        m_pBuffer = new char[buffLen];
        DMFTCHECKNULL_GOTO(m_pBuffer, done, E_OUTOFMEMORY);
        m_pBuffer[0] = 0;
        for (UINT32 ulIndex = 0; ulIndex < attrCount; ulIndex++)
        {
            PropVariantInit(&var);
            checkAdjustBufferCap(m_pBuffer, buffLen);
            DMFTCHECKHR_GOTO(pMediaType->GetItemByIndex(ulIndex, &attrGuid, &var), done);
            DMFTCHECKHR_GOTO(pMediaType->GetItemType(attrGuid, &pType), done);
            if (ulIndex > 0)
                strcat_s(m_pBuffer, MEDIAPRINTER_STARTLEN, " : ");
            strcat_s(m_pBuffer, buffLen, GetGUIDNameConst(attrGuid));
            strcat_s(m_pBuffer, buffLen, "=");
            pTempBaseStr = DumpAttribute(pType, var);
            strcat_s(m_pBuffer, buffLen, pTempBaseStr);
            delete(pTempBaseStr);
            PropVariantClear(&var);
        }
    done:
        if (tempStore)
        {
            delete(tempStore);
        }
    }
    return m_pBuffer;
}

/*++
Description:
Rudimentary function to print the Media type
--*/

PCHAR CMediaTypePrinter::ToString()
{
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
        DMFTCHECKNULL_GOTO(m_pBuffer, done, E_OUTOFMEMORY);
        m_pBuffer[0] = 0;
        for (UINT32 ulIndex = 0; ulIndex < ARRAYSIZE(impGuids); ulIndex++)
        {
            PropVariantInit(&var);
            checkAdjustBufferCap(m_pBuffer, buffLen);
            attrGuid = impGuids[ulIndex];
            DMFTCHECKHR_GOTO(pMediaType->GetItemType(attrGuid, &pType), done);
            DMFTCHECKHR_GOTO(pMediaType->GetItem(attrGuid, &var), done);
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
done:
    return m_pBuffer;
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
            mf_assert(mbGuidLen == (int)wcslen(lpszGuidString));
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

void printMessageEvent(MFT_MESSAGE_TYPE msg)
{
    switch (msg)
    {
    case MFT_MESSAGE_COMMAND_FLUSH:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! :PROCESSMESSAGE: MFT_MESSAGE_COMMAND_FLUSH");
        break;
    case MFT_MESSAGE_COMMAND_DRAIN:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! :PROCESSMESSAGE: MFT_MESSAGE_COMMAND_DRAIN");
        break;
    case MFT_MESSAGE_COMMAND_MARKER:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! :PROCESSMESSAGE: MFT_MESSAGE_COMMAND_MARKER");
        break;
    case MFT_MESSAGE_COMMAND_TICK:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! :PROCESSMESSAGE: MFT_MESSAGE_COMMAND_TICK");
        break;
    case MFT_MESSAGE_NOTIFY_END_OF_STREAM:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! :PROCESSMESSAGE: MFT_MESSAGE_NOTIFY_END_OF_STREAM");
        break;
    case MFT_MESSAGE_NOTIFY_BEGIN_STREAMING:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! :PROCESSMESSAGE: MFT_MESSAGE_NOTIFY_BEGIN_STREAMING");
        break;
    case MFT_MESSAGE_NOTIFY_START_OF_STREAM:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! :PROCESSMESSAGE: MFT_MESSAGE_NOTIFY_START_OF_STREAM");
        break;
    case  MFT_MESSAGE_DROP_SAMPLES:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! :PROCESSMESSAGE: MFT_MESSAGE_DROP_SAMPLES");
        break;
    case MFT_MESSAGE_SET_D3D_MANAGER:
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! :PROCESSMESSAGE: MFT_MESSAGE_SET_D3D_MANAGER");
        break;

    }
}