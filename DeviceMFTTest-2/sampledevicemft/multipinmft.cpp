//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media Foundation
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//

#include "stdafx.h"
#include "multipinmft.h"
#ifdef MF_WPP
#include "multipinmft.tmh"    //--REF_ANALYZER_DONT_REMOVE--
#endif
//
// Note since MFT_UNIQUE_METHOD_NAMES is defined all the functions of IMFTransform have the Mft suffix..
//
extern const CLSID CLSID_HwMFTActivate;

#include <array>

std::string guidToString2(GUID guid) {
    std::array<char, 40> output;
    snprintf(output.data(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    return std::string(output.data());
}

CMultipinMft::CMultipinMft()
:   m_nRefCount( 0 ),
    m_InputPinCount( 0 ),
    m_OutputPinCount( 0 ),
    m_dwWorkQueueId ( MFASYNC_CALLBACK_QUEUE_MULTITHREADED ),
    m_lWorkQueuePriority ( 0 ),
    m_spAttributes( nullptr ),
    m_spSourceTransform( nullptr ),
    m_SymbolicLink(nullptr)
{
    HRESULT hr = S_OK;
    ComPtr<IMFAttributes> pAttributes = nullptr;
    MFCreateAttributes( &pAttributes, 0 );
    DMFTCHECKHR_GOTO(pAttributes->SetUINT32( MF_TRANSFORM_ASYNC, TRUE ),done);
    DMFTCHECKHR_GOTO(pAttributes->SetUINT32( MFT_SUPPORT_DYNAMIC_FORMAT_CHANGE, TRUE ),done);
    DMFTCHECKHR_GOTO(pAttributes->SetUINT32( MF_SA_D3D_AWARE, TRUE ), done);
    DMFTCHECKHR_GOTO(pAttributes->SetString( MFT_ENUM_HARDWARE_URL_Attribute, L"SampleMultiPinMft" ),done);
    m_spAttributes = pAttributes;
done:
    if (FAILED(hr))
    {

    }
}

CMultipinMft::~CMultipinMft( )
{
    

    for ( ULONG ulIndex = 0, ulSize = (ULONG) m_InPins.size(); ulIndex < ulSize; ulIndex++ )
    {
        SAFERELEASE(m_InPins[ ulIndex ]);
    }
    m_InPins.clear();
    for (ULONG ulIndex = 0, ulSize = (ULONG) m_OutPins.size(); ulIndex < ulSize; ulIndex++)
    {
        SAFERELEASE(m_OutPins[ ulIndex ]);
    }
    m_OutPins.clear();
    SAFE_ARRAYDELETE(m_SymbolicLink);
    m_spSourceTransform = nullptr;

}

STDMETHODIMP_(ULONG) CMultipinMft::AddRef(
    void
    )
{
    return InterlockedIncrement(&m_nRefCount);
}

STDMETHODIMP_(ULONG) CMultipinMft::Release(
    void
    )
{
    ULONG uCount = InterlockedDecrement(&m_nRefCount);

    if ( uCount == 0 )
    {
        delete this;
    }
    return uCount;
}

STDMETHODIMP CMultipinMft::QueryInterface(
    _In_ REFIID iid,
    _COM_Outptr_ void** ppv
    )
{

    HRESULT hr = S_OK;
    *ppv = NULL;
    
    if (iid == __uuidof(IUnknown))
    {
        *ppv = static_cast< IMFDeviceTransform* >(this);
    }
    else if (iid == __uuidof(IMFDeviceTransform))
    {
        *ppv = static_cast<IMFDeviceTransform*>(this);
    }
    else if ( iid == __uuidof( IMFMediaEventGenerator ) )
    {
        *ppv = static_cast< IMFMediaEventGenerator* >(this);
    }
    else if ( iid == __uuidof( IMFShutdown ) )
    {
        *ppv = static_cast< IMFShutdown* >( this );
    }
#if defined (MF_DEVICEMFT_ALLOW_MFT0_LOAD) && defined (MFT_UNIQUE_METHOD_NAMES)
    else if (iid == __uuidof(IMFTransform))
    {
        *ppv = static_cast< IMFTransform* >(this);
    }
#endif
    else if ( iid == __uuidof( IKsControl ) )
    {
        *ppv = static_cast< IKsControl* >( this );
    }
    else if ( iid == __uuidof( IMFRealTimeClientEx ) )
    {
        *ppv = static_cast< IMFRealTimeClientEx* >( this );
    }
    else
    {
        hr = E_NOINTERFACE;
        goto done;
    }
    AddRef();
done:
    return hr;
}

/*++
    Description:
    This function is the entry point of the transform
    The following things may be initialized here
    1) Query for MF_DEVICEMFT_CONNECTED_FILTER_KSCONTROL on the attributes supplied 
    2) From the IUnknown acquired get the IMFTransform interface.
    3) Get the stream count.. The output streams are of consequence to the tranform.
    The input streams should correspond to the output streams exposed by the source transform
    acquired from the Attributes supplied.
    4) Get the IKSControl which is used to send KSPROPERTIES, KSEVENTS and KSMETHODS to the driver for the filer level. Store it in your filter class
    5) Get the OutPutStreamAttributes for the output pins of the source transform.  This can further be used to QI and acquire
     the IKSControl related to the specific pin. This can be used to send PIN level KSPROPERTIES, EVENTS and METHODS to the pins
    6) Create the output pins

--*/

// This sample will create a grayscale for known media types. Please remove MF_DEVICEMFT_ADD_GRAYSCALER_ to remove the grayscaler
// This sample also has photo confirmation enabled remove DMF_DEVICEMFT_PHTOTOCONFIRMATION to remove photo confirmation
// Please search for the @@@@ README tag for critical sections in code and it's documentation
//
STDMETHODIMP CMultipinMft::InitializeTransform ( 
    _In_ IMFAttributes *pAttributes
    )
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::InitializeTransform -----");

    HRESULT                 hr              = S_OK;
    ComPtr<IUnknown>        spFilterUnk     = nullptr;
    DWORD                   *pcInputStreams = NULL, *pcOutputStreams = NULL;
    DWORD                   inputStreams    = 0;
    DWORD                   outputStreams   = 0;
    GUID*                   outGuids        = NULL;
    GUID                    streamCategory  = GUID_NULL;
    ULONG                   ulOutPinIndex   = 0;
    UINT32                  uiSymLinkLen    = 0;
    CPinCreationFactory*    pPinFactory = new (std::nothrow) CPinCreationFactory(this);
    DMFTCHECKNULL_GOTO( pAttributes, done, E_INVALIDARG );
    //
    // The attribute passed with MF_DEVICEMFT_CONNECTED_FILTER_KSCONTROL is the source transform. This generally represents a filter
    // This needs to be stored so that we know the device properties. We cache it. We query for the IKSControl which is used to send
    // controls to the driver.
    //
    DMFTCHECKHR_GOTO( pAttributes->GetUnknown( MF_DEVICEMFT_CONNECTED_FILTER_KSCONTROL,IID_PPV_ARGS( &spFilterUnk ) ),done );
    
    if (SUCCEEDED(pAttributes->GetStringLength(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, &uiSymLinkLen))) // Not available prior to RS5
    {
        m_SymbolicLink = new (std::nothrow) WCHAR[++uiSymLinkLen];
        DMFTCHECKNULL_GOTO(m_SymbolicLink, done, E_OUTOFMEMORY);
        DMFTCHECKHR_GOTO(pAttributes->GetString(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, m_SymbolicLink, uiSymLinkLen, &uiSymLinkLen), done);
    }

    DMFTCHECKHR_GOTO( spFilterUnk.As( &m_spSourceTransform ), done );
    
    DMFTCHECKHR_GOTO( m_spSourceTransform.As( &m_spIkscontrol ), done );
    
    DMFTCHECKHR_GOTO( m_spSourceTransform->MFTGetStreamCount( &inputStreams, &outputStreams ), done );

    spFilterUnk = nullptr;

    //
    //The number of input pins created by the device transform should match the pins exposed by
    //the source transform i.e. outputStreams from SourceTransform or DevProxy = Input pins of the Device MFT
    //
    
    if ( inputStreams > 0 || outputStreams > 0 )
    {
        pcInputStreams = new (std::nothrow) DWORD[ inputStreams ];
        DMFTCHECKNULL_GOTO( pcInputStreams, done, E_OUTOFMEMORY);

        pcOutputStreams = new (std::nothrow) DWORD[ outputStreams ];
        DMFTCHECKNULL_GOTO( pcOutputStreams, done, E_OUTOFMEMORY );
        
        DMFTCHECKHR_GOTO( m_spSourceTransform->MFTGetStreamIDs( inputStreams, pcInputStreams,
            outputStreams,
            pcOutputStreams ),done );

        //
        // Output pins from DevProxy = Input pins of device MFT.. We are the first transform in the pipeline before MFT0
        //
        
        for ( ULONG ulIndex = 0; ulIndex < outputStreams; ulIndex++ )
        {           
            ComPtr<IMFAttributes>   pInAttributes   = nullptr;
            BOOL                    bCustom         = FALSE;
            ComPtr<CInPin>          spInPin;

            DMFTCHECKHR_GOTO(pPinFactory->CreatePin(
                pcOutputStreams[ulIndex], /*Input Pin ID as advertised by the pipeline*/
                0, /*This is not needed for Input Pin*/
                CPinCreationFactory::DMFT_PIN_INPUT, /*Input Pin*/
                (CBasePin**)spInPin.GetAddressOf(),
                bCustom), done);
            if (bCustom)
            {
                m_CustomPinCount++;
            }
            hr = ExceptionBoundary([&]()
            {
                m_InPins.push_back(spInPin.Get());
            });
            DMFTCHECKHR_GOTO(hr, done);
            DMFTCHECKHR_GOTO( spInPin->Init(m_spSourceTransform.Get() ), done);
            spInPin.Detach();
        }
        
        //
        // Create one on one mapping
        //
        for (ULONG ulIndex = 0; ulIndex < m_InPins.size(); ulIndex++)
        {
            
            ComPtr<COutPin> spoPin;
            BOOL     bCustom = FALSE;
            ComPtr<CInPin> spiPin = ( CInPin * )m_InPins[ ulIndex ];
            
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
    
    m_InputPinCount =  ULONG ( m_InPins.size() );
    m_OutputPinCount = ULONG ( m_OutPins.size() );
    
done:
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! -----",hr,hr);
 
    if ( pcInputStreams )
    {
        delete[ ] ( pcInputStreams );
    }
    if ( pcOutputStreams )
    {
        delete[ ] ( pcOutputStreams );
    }
    if ( outGuids )
    {
        delete [] ( outGuids );
    }
    SAFE_DELETE(pPinFactory);
    if ( FAILED( hr ) )
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


STDMETHODIMP CMultipinMft::SetWorkQueueEx(
    _In_  DWORD dwWorkQueueId,
    _In_ LONG lWorkItemBasePriority
    )
/*++
    Description:

    Implements IMFRealTimeClientEx::SetWorkQueueEx function

--*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetWorkQueueEx -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetWorkQueueEx dwWorkQueueId: %d", dwWorkQueueId);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetWorkQueueEx lWorkItemBasePriority: %d", lWorkItemBasePriority);


    CAutoLock   lock( m_critSec );
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

    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetWorkQueueEx Exit -----");
    return S_OK;

}

//
// IMFDeviceTransform functions
//
STDMETHODIMP  CMultipinMft::GetStreamCount(
    _Inout_ DWORD  *pdwInputStreams,
    _Inout_ DWORD  *pdwOutputStreams
    )
/*++
    Description:    Implements IMFTransform::GetStreamCount function
--*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetStreamCount -----");

    HRESULT hr = S_OK;
    CAutoLock   lock(m_critSec);
    DMFTCHECKNULL_GOTO(pdwInputStreams, done, E_INVALIDARG);
    DMFTCHECKNULL_GOTO(pdwOutputStreams, done, E_INVALIDARG);
    *pdwInputStreams     = m_InputPinCount;
    *pdwOutputStreams    = m_OutputPinCount;

done:
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;

}

//
//Doesn't strictly conform to the GetStreamIDs on IMFTransform Interface!
//
STDMETHODIMP  CMultipinMft::GetStreamIDs(
    _In_                                    DWORD  dwInputIDArraySize,
    _When_(dwInputIDArraySize >= m_InputPinCount, _Out_writes_(dwInputIDArraySize))  DWORD* pdwInputIDs,
    _In_                                    DWORD  dwOutputIDArraySize,
    _When_(dwOutputIDArraySize >= m_OutputPinCount && (pdwInputIDs && (dwInputIDArraySize > 0)),
    _Out_writes_(dwOutputIDArraySize)) _On_failure_(_Valid_) DWORD* pdwOutputIDs
    )
/*++
    Description:
        Implements IMFTransform::GetStreamIDs function
--*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetStreamIDs -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetStreamIDs dwInputIDArraySize: %d", dwInputIDArraySize);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetStreamIDs dwOutputIDArraySize: %d", dwOutputIDArraySize);

    HRESULT hr = S_OK;
    CAutoLock   lock(m_critSec);
    if ( ( dwInputIDArraySize < m_InputPinCount ) && ( dwOutputIDArraySize < m_OutputPinCount ) )
    {
        hr = MF_E_BUFFERTOOSMALL;
        goto done;
    }

    if ( dwInputIDArraySize )
    {
        DMFTCHECKNULL_GOTO( pdwInputIDs, done, E_POINTER );
        for ( DWORD dwIndex = 0; dwIndex < ((dwInputIDArraySize > m_InputPinCount) ? m_InputPinCount:
            dwInputIDArraySize); dwIndex++ )
        {
            pdwInputIDs[ dwIndex ] = ( m_InPins[dwIndex] )->streamId();
        }
    }
    
    if ( dwOutputIDArraySize )
    {
        DMFTCHECKNULL_GOTO( pdwOutputIDs, done, E_POINTER );
        for ( DWORD dwIndex = 0; dwIndex < ((dwOutputIDArraySize >  m_OutputPinCount)? m_OutputPinCount:
            dwOutputIDArraySize); dwIndex++ )
        {
            pdwOutputIDs[ dwIndex ] = (m_OutPins[ dwIndex ])->streamId();
        }
    }
done:
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;

}

/*++
Name: CMultipinMft::GetInputAvailableType
Description:
Implements IMFTransform::GetInputAvailableType function. This function
gets the media type supported by the specified stream based on the
index dwTypeIndex.
--*/
STDMETHODIMP  CMultipinMft::GetInputAvailableType(
    _In_        DWORD           dwInputStreamID,
    _In_        DWORD           dwTypeIndex,
    _Out_ IMFMediaType**        ppMediaType
    )
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetInputAvailableType -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetInputAvailableType dwInputStreamID: %d", dwInputStreamID);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetInputAvailableType dwTypeIndex: %d", dwTypeIndex);


    HRESULT hr = S_OK;
        
    ComPtr<CInPin> spiPin = GetInPin( dwInputStreamID );
    DMFTCHECKNULL_GOTO(ppMediaType, done, E_INVALIDARG);
    DMFTCHECKNULL_GOTO( spiPin, done, MF_E_INVALIDSTREAMNUMBER );
    
    *ppMediaType = nullptr;

    hr = spiPin->GetOutputAvailableType( dwTypeIndex,ppMediaType );

    if (FAILED(hr))
    {
        DMFTRACE( DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! Pin: %d Index: %d exiting  %!HRESULT!",
            dwInputStreamID,
            dwTypeIndex,
            hr);
    }

done:
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;

}

STDMETHODIMP CMultipinMft::GetOutputAvailableType(
    _In_         DWORD           dwOutputStreamID,
    _In_         DWORD           dwTypeIndex,
    _Out_        IMFMediaType**  ppMediaType
    )
/*++
    Description:

        Implements IMFTransform::GetOutputAvailableType function. This function
        gets the media type supported by the specified stream based on the
        index dwTypeIndex.

--*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetOutputAvailableType -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetOutputAvailableType dwOutputStreamID: %d", dwOutputStreamID);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetOutputAvailableType dwTypeIndex: %d", dwTypeIndex);

    HRESULT hr = S_OK;
    CAutoLock Lock(m_critSec);

    ComPtr<COutPin> spoPin = GetOutPin( dwOutputStreamID );

    DMFTCHECKNULL_GOTO( spoPin.Get(), done, MF_E_INVALIDSTREAMNUMBER );
    DMFTCHECKNULL_GOTO(ppMediaType, done, E_INVALIDARG);

    *ppMediaType = nullptr;

    hr = spoPin->GetOutputAvailableType( dwTypeIndex, ppMediaType );

    if ( FAILED( hr ) )
    {
        DMFTRACE( DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! Pin: %d Index: %d exiting  %!HRESULT!", 
            dwOutputStreamID,
            dwTypeIndex,
            hr );
    }

done:
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;

}

STDMETHODIMP  CMultipinMft::GetInputCurrentType(
    _In_        DWORD           dwInputStreamID,
    _COM_Outptr_result_maybenull_ IMFMediaType**  ppMediaType
    )
/*++
    Description:
        Implements IMFTransform::GetInputCurrentType function. This function
        returns the current media type set on the specified stream.
--*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetInputCurrentType -----");

    //
    // The input current types will not come to this transform.
    // The outputs of this transform matter. The DTM manages the
    // output of this transform and the inptuts of the source transform
    //
    UNREFERENCED_PARAMETER(dwInputStreamID);
    UNREFERENCED_PARAMETER(ppMediaType);
    return S_OK;
}

STDMETHODIMP  CMultipinMft::GetOutputCurrentType(
    _In_         DWORD           dwOutputStreamID,
    _Out_        IMFMediaType**  ppMediaType
    )
/*++
    Description:

        Implements IMFTransform::GetOutputCurrentType function. This function
        returns the current media type set on the specified stream.

--*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetOutputCurrentType -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetOutputCurrentType dwOutputStreamID: %d", dwOutputStreamID);

    HRESULT hr = S_OK;
    ComPtr<COutPin> spoPin;
    CAutoLock lock( m_critSec );

    DMFTCHECKNULL_GOTO( ppMediaType, done, E_INVALIDARG );
    
    *ppMediaType = nullptr;

    spoPin = GetOutPin( dwOutputStreamID );

    DMFTCHECKNULL_GOTO(spoPin, done, MF_E_INVALIDSTREAMNUMBER );

    DMFTCHECKHR_GOTO(spoPin->getMediaType( ppMediaType ),done );
    
    DMFTCHECKNULL_GOTO( *ppMediaType, done, MF_E_TRANSFORM_TYPE_NOT_SET );

done:
    DMFTRACE( DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr );
    return hr;

}


STDMETHODIMP  CMultipinMft::ProcessEvent(
    _In_    DWORD           dwInputStreamID,
    _In_    IMFMediaEvent*  pEvent
    )
    /*++
    Description:

    Implements IMFTransform::ProcessEvent function. This function
    processes events that come to the MFT.

    --*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::ProcessEvent -----");

    UNREFERENCED_PARAMETER(dwInputStreamID);
    UNREFERENCED_PARAMETER(pEvent);
    return S_OK;
}



STDMETHODIMP  CMultipinMft::ProcessMessage(
    _In_ MFT_MESSAGE_TYPE    eMessage,
    _In_ ULONG_PTR           ulParam
    )
/*++
    Description:

        Implements IMFTransform::ProcessMessage function. This function
        processes messages coming to the MFT.

--*/
{
    UNREFERENCED_PARAMETER(ulParam);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::ProcessMessage -----");
    printMessageEvent(eMessage);

    return S_OK;
}

STDMETHODIMP  CMultipinMft::ProcessInput(
    _In_ DWORD      dwInputStreamID,
    _In_ IMFSample* pSample,
    _In_ DWORD      dwFlags
    )
/*++
    Description:

        Implements IMFTransform::ProcessInput function.This function is called
        when the sourcetransform has input to feed. the pins will try to deliver the 
        samples to the active output pins conencted. if none are connected then just
        returns the sample back to the source transform

--*/
{
    UNREFERENCED_PARAMETER(pSample);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::ProcessInput -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::ProcessInput dwInputStreamID: %d", dwInputStreamID);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::ProcessInput dwFlags: %d", dwFlags);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::ProcessInput E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP  CMultipinMft::ProcessOutput(
    _In_    DWORD                       dwFlags,
    _In_    DWORD                       cOutputBufferCount,
    _Inout_updates_(cOutputBufferCount)  MFT_OUTPUT_DATA_BUFFER  *pOutputSamples,
    _Out_   DWORD                       *pdwStatus
)
/*++
Description:

Implements IMFTransform::ProcessOutput function. This is called by the DTM when
the DT indicates it has samples to give. The DTM will send enough MFT_OUTPUT_DATA_BUFFER
pointers to be filled up as is the number of output pins available. The DT should traverse its
output pins and populate the corresponding MFT_OUTPUT_DATA_BUFFER with the samples available

--*/
{
    UNREFERENCED_PARAMETER(pOutputSamples);
    UNREFERENCED_PARAMETER(pdwStatus);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::ProcessOutput -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::ProcessOutput dwFlags: %d", dwFlags);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::ProcessOutput cOutputBufferCount: %d", cOutputBufferCount);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::ProcessOutput E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP  CMultipinMft::GetInputStreamAttributes(
    _In_        DWORD           dwInputStreamID,
    _COM_Outptr_result_maybenull_ IMFAttributes** ppAttributes
    )
/*++
    Description:

        Implements IMFTransform::GetInputStreamAttributes function. This function
        gets the specified input stream's attributes.

--*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetInputStreamAttributes -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetInputStreamAttributes dwInputStreamID: %d", dwInputStreamID);

    HRESULT hr = S_OK;
    ComPtr<CInPin> spIPin;

    DMFTCHECKNULL_GOTO( ppAttributes, done, E_INVALIDARG );
    *ppAttributes = nullptr;

    spIPin = GetInPin( dwInputStreamID );

    DMFTCHECKNULL_GOTO(spIPin, done, E_INVALIDARG );

    hr  = spIPin->getPinAttributes(ppAttributes);

done:
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;

}

STDMETHODIMP  CMultipinMft::GetOutputStreamAttributes(
    _In_        DWORD           dwOutputStreamID,
    _Out_ IMFAttributes** ppAttributes
    )
/*++
    Description:

        Implements IMFTransform::GetOutputStreamAttributes function. This function
        gets the specified output stream's attributes.

--*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetOutputStreamAttributes -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetOutputStreamAttributes dwOutputStreamID: %d", dwOutputStreamID);

    HRESULT hr = S_OK;
    ComPtr<COutPin> spoPin;

    DMFTCHECKNULL_GOTO(ppAttributes, done, E_INVALIDARG);

    *ppAttributes = nullptr;

    spoPin = GetOutPin(dwOutputStreamID);

    DMFTCHECKNULL_GOTO(spoPin, done, E_INVALIDARG );

    DMFTCHECKHR_GOTO(spoPin->getPinAttributes(ppAttributes), done );
done:
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;
}

_Requires_no_locks_held_
STDMETHODIMP CMultipinMft::SetInputStreamState(
    _In_    DWORD               dwStreamID,
    _In_    IMFMediaType        *pMediaType,
    _In_    DeviceStreamState   value,
    _In_    DWORD               dwFlags
    )
    /*++
    Description:

    Implements IMFdeviceTransform::SetInputStreamState function. 
    Sets the input stream state. 

    The control lock is not taken here. The lock is taken for operations on
    output pins. This operation is a result of the DT notifying the DTM that
    output pin change has resulted in the need for the input to be changed. In
    this case the DTM sends a getpreferredinputstate and then this call

    --*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetInputStreamState -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetInputStreamState dwStreamID: %d", dwStreamID);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetInputStreamState value: %d", value);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetInputStreamState dwFlags: %d", dwFlags);

    HRESULT hr = S_OK;
    ComPtr<CInPin> spiPin = GetInPin(dwStreamID);
    DMFTCHECKNULL_GOTO(spiPin, done, MF_E_INVALIDSTREAMNUMBER);
    
    DMFTCHECKHR_GOTO(spiPin->SetInputStreamState(pMediaType, value, dwFlags),done);
   
done:
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;

}

STDMETHODIMP CMultipinMft::GetInputStreamState(
    _In_    DWORD               dwStreamID,
    _Out_   DeviceStreamState   *value
    )
{
    UNREFERENCED_PARAMETER(value);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetInputStreamState -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetInputStreamState dwStreamID: %d", dwStreamID);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetInputStreamState E_NOTIMPL -----");
    return E_NOTIMPL;
}


STDMETHODIMP CMultipinMft::SetOutputStreamState(
    _In_ DWORD                  dwStreamID,
    _In_ IMFMediaType           *pMediaType,
    _In_ DeviceStreamState      state,
    _In_ DWORD                  dwFlags
    )
    /*++
    Description:

    Implements IMFdeviceTransform::SetOutputStreamState function.
    Sets the output stream state. This is called whenever the stream
    is selected or deslected i.e. started or stopped.

    The control lock taken here and this operation should be atomic.
    This function should check the input pins connected to the output pin
    switch off the state of the input pin. Check if any other Pin connected
    to the input pin is in a conflicting state with the state requested on this
    output pin. Accordinly it calculates the media type to be set on the input pin
    and the state to transition into. It then might recreate the other output pins
    connected to it
    --*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetOutputStreamState -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetOutputStreamState dwStreamID: %d", dwStreamID);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetOutputStreamState state: %d", state);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetOutputStreamState dwFlags: %d", dwFlags);

    HRESULT hr = S_OK;
    UNREFERENCED_PARAMETER(dwFlags);
    CAutoLock Lock(m_critSec);

    DMFTCHECKHR_GOTO(ChangeMediaTypeEx(dwStreamID, pMediaType, state),done);
   
done:    
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
    return hr;
}

STDMETHODIMP CMultipinMft::GetOutputStreamState(
    _In_    DWORD               dwStreamID,
    _Out_   DeviceStreamState   *pState
    )
    /*++
    Description:

    Implements IMFdeviceTransform::GetOutputStreamState function.
    Gets the output stream state.
    Called by the DTM to checks states. Atomic operation. needs a lock
    --*/
{
    UNREFERENCED_PARAMETER(pState);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetOutputStreamState -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetOutputStreamState dwStreamID: %d", dwStreamID);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetOutputStreamState E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP CMultipinMft::GetInputStreamPreferredState(
    _In_              DWORD                             dwStreamID,
    _Inout_           DeviceStreamState                 *value,
    _Outptr_opt_result_maybenull_ IMFMediaType          **ppMediaType
    )
    /*++
    Description:

    Implements IMFdeviceTransform::GetInputStreamPreferredState function.
    Gets the preferred state and the media type to be set on the input pin.
    The lock is not held as this will always be called only when we notify
    DTM to call us. We notify DTM only from the context on operations 
    happening on the output pin
    --*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetInputStreamPreferredState -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetInputStreamPreferredState dwStreamID: %d", dwStreamID);

    HRESULT hr = S_OK;
    ComPtr<CInPin> spiPin = GetInPin(dwStreamID);
    DMFTCHECKNULL_GOTO(ppMediaType, done, E_INVALIDARG);
    DMFTCHECKNULL_GOTO(spiPin, done, MF_E_INVALIDSTREAMNUMBER);
    hr = spiPin->GetInputStreamPreferredState(value, ppMediaType);
done:
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! -----", hr, hr);
   return hr;
}

STDMETHODIMP CMultipinMft::FlushInputStream(
    _In_ DWORD dwStreamIndex,
    _In_ DWORD dwFlags
    )
    /*++
    Description:

    Implements IMFdeviceTransform::FlushInputStream function.
    --*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::FlushInputStream -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::FlushInputStream dwStreamIndex: %d", dwStreamIndex);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::FlushInputStream dwFlags: %d", dwFlags);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::FlushInputStream E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP CMultipinMft::FlushOutputStream(
    _In_ DWORD                  dwStreamIndex,
    _In_ DWORD                  dwFlags
    )
    /*++
    Description:

    Implements IMFdeviceTransform::FlushOutputStream function.
    Called by the DTM to flush streams
    --*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::FlushOutputStream -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::FlushOutputStream dwStreamIndex: %d", dwStreamIndex);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::FlushOutputStream dwFlags: %d", dwFlags);
    return E_NOTIMPL;
}


/*++
    Description:

    Called when the Device Transform gets a MFT_MESSAGE_COMMAND_FLUSH. We drain all the queues. 
    This is called in device source when the source gets end of streaming.
    --*/
STDMETHODIMP_(VOID) CMultipinMft::FlushAllStreams(
    VOID
    )
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::FlushAllStreams -----");
}


//
// IKsControl interface functions
//
STDMETHODIMP CMultipinMft::KsProperty(
    _In_reads_bytes_(ulPropertyLength) PKSPROPERTY pProperty,
    _In_ ULONG ulPropertyLength,
    _Inout_updates_bytes_(ulDataLength) LPVOID pvPropertyData,
    _In_ ULONG ulDataLength,
    _Inout_ ULONG* pulBytesReturned
    )
    /*++
    Description:

    Implements IKSProperty::KsProperty function.
    used to pass control commands to the driver (generally)
    This can be used to intercepted the control to figure out
    if it needs to be propogated to the driver or not
    --*/
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::KsProperty -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::KsProperty ulPropertyLength: %d", ulPropertyLength);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::KsProperty ulDataLength: %d", ulDataLength);


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
        DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! Warm Start Control %d Passed ", pProperty->Id);
    }

    if (IsEqualCLSID(pProperty->Set, KSPROPERTYSETID_ExtendedCameraControl))
    {
        DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! Extended Control %d Passed ",pProperty->Id);
    }
    else if ((IsEqualCLSID(pProperty->Set, PROPSETID_VIDCAP_VIDEOCONTROL)) && (pProperty->Id == KSPROPERTY_VIDEOCONTROL_MODE))
    {
        // A function illustrating how we can capture and service photos from the device MFT. This block shows how we can
        // intercept Photo triggers going down to the pipeline
        
        if (sizeof(KSPROPERTY_VIDEOCONTROL_MODE_S) == ulDataLength)
        {
            PKSPROPERTY_VIDEOCONTROL_MODE_S VideoControl = (PKSPROPERTY_VIDEOCONTROL_MODE_S)pvPropertyData;
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
        }
    }
    DMFTCHECKHR_GOTO(m_spIkscontrol->KsProperty(pProperty,
        ulPropertyLength,
        pvPropertyData,
        ulDataLength,
        pulBytesReturned),done);
done:
    LPSTR guidStr = DumpGUIDA(pProperty->Set);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! g:%s p:%d exiting %x = %!HRESULT! -----", guidStr, pProperty->Id, hr, hr);
    delete(guidStr);
    return hr;

}

STDMETHODIMP CMultipinMft::KsMethod(
    _In_reads_bytes_(ulPropertyLength) PKSMETHOD   pMethod,
    _In_ ULONG ulPropertyLength,
    _Inout_updates_bytes_(ulDataLength) LPVOID pvPropertyData,
    _In_ ULONG ulDataLength,
    _Inout_ ULONG* pulBytesReturned
    )
    /*++
    Description:

    Implements IKSProperty::KsMethod function. We can trap ksmethod calls here.
    --*/
{
    UNREFERENCED_PARAMETER(pMethod);
    UNREFERENCED_PARAMETER(pvPropertyData);
    UNREFERENCED_PARAMETER(pulBytesReturned);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::KsMethod -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::KsMethod ulPropertyLength: %d", ulPropertyLength);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::KsMethod ulDataLength: %d", ulDataLength);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::KsMethod E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP CMultipinMft::KsEvent(
    _In_reads_bytes_(ulEventLength) PKSEVENT pEvent,
    _In_ ULONG ulEventLength,
    _Inout_updates_bytes_opt_(ulDataLength) LPVOID pEventData,
    _In_ ULONG ulDataLength,
    _Inout_ ULONG* pBytesReturned
    )
    /*++
    Description:

    Implements IKSProperty::KsEvent function.
    --*/
{
    UNREFERENCED_PARAMETER(pEvent);
    UNREFERENCED_PARAMETER(pEventData);
    UNREFERENCED_PARAMETER(pBytesReturned);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::KsEvent -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::KsEvent ulEventLength: %d", ulEventLength);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::KsEvent ulDataLength: %d", ulDataLength);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::KsEvent E_NOTIMPL -----");
    return E_NOTIMPL;
}


#if defined (MF_DEVICEMFT_ALLOW_MFT0_LOAD) && defined (MFT_UNIQUE_METHOD_NAMES)

//
// IMFTransform function(s).
//

//
// Note: This is the only IMFTransform function which is not a redirector to the 
// DeviceTransform functions. The rest of IMFTransform functions are in the file common.h
// This function returns the IMFAttribute created for Device MFT. If DMFT is 
// not loaded (usually )MFT0's call to GetAttributes will get the Attribute store of DevProxy.
// A device MFT loaded will not pass through the devproxy attribute store, but it will pass 
// the device MFT attributes. This should be similar to the singular DevProxy attribute
// which the MFT0 providers can use to synchronize across various MFT0's
//

STDMETHODIMP CMultipinMft::GetAttributes(
    _COM_Outptr_opt_result_maybenull_ IMFAttributes** ppAttributes
    )
{
    UNREFERENCED_PARAMETER(ppAttributes);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetAttributes -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetAttributes E_NOTIMPL -----");
    return E_NOTIMPL;
}
#endif

#if ((defined NTDDI_WIN10_VB) && (NTDDI_VERSION >= NTDDI_WIN10_VB))
//
// IMFSampleAllocatorControl Inferface function declarations
//

STDMETHODIMP CMultipinMft::SetDefaultAllocator(
    _In_ DWORD dwOutputStreamID,
    _In_ IUnknown *pAllocator
)
{
    UNREFERENCED_PARAMETER(pAllocator);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetDefaultAllocator -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetDefaultAllocator dwOutputStreamID: %d", dwOutputStreamID);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SetDefaultAllocator E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP CMultipinMft::GetAllocatorUsage(
    _In_ DWORD dwOutputStreamID,
    _Out_  DWORD* pdwInputStreamID,
    _Out_ MFSampleAllocatorUsage* peUsage
)
{
    UNREFERENCED_PARAMETER(pdwInputStreamID);
    UNREFERENCED_PARAMETER(peUsage);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetAllocatorUsage -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetAllocatorUsage dwOutputStreamID: %d", dwOutputStreamID);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetAllocatorUsage E_NOTIMPL -----");
    return E_NOTIMPL;
}

#endif // ((defined NTDDI_WIN10_VB) && (NTDDI_VERSION >= NTDDI_WIN10_VB))

//
// HELPER FUNCTIONS
//

//
// A lock here could mean a deadlock because this will be called when the lock is already held
// in another thread.
//
CInPin* CMultipinMft::GetInPin( 
    _In_ DWORD dwStreamId
)
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetInPin");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetInPin dwStreamId: %d", dwStreamId);

    CInPin *inPin = NULL;
    for (DWORD dwIndex = 0, dwSize = (DWORD)m_InPins.size(); dwIndex < dwSize; dwIndex++)
    {
         inPin = (CInPin *)m_InPins[dwIndex];
        if (dwStreamId == inPin->streamId())
        {
            break;
        }
        inPin = NULL;
    }
    return inPin;

}

COutPin* CMultipinMft::GetOutPin(
    _In_ DWORD dwStreamId
    )
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetOutPin");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetOutPin dwStreamId: %d", dwStreamId);

    COutPin *outPin = NULL;
    for ( DWORD dwIndex = 0, dwSize = (DWORD) m_OutPins.size(); dwIndex < dwSize; dwIndex++ )
    {
        outPin = ( COutPin * )m_OutPins[ dwIndex ];

        if ( dwStreamId == outPin->streamId() )
        {
            break;
        }

        outPin = NULL;
    }

    return outPin;

}

_Requires_lock_held_(m_Critsec)
HRESULT CMultipinMft::GetConnectedInpin(_In_ ULONG ulOutpin, _Out_ ULONG &ulInPin)
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetConnectedInpin");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::GetConnectedInpin ulOutpin: %d", ulOutpin);

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

    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_ERROR, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;

}

//
// The Below function changes media type on the pins exposed by device MFT
//
__requires_lock_held(m_critSec)
HRESULT CMultipinMft::ChangeMediaTypeEx(
    _In_        ULONG              pinId,
    _In_opt_    IMFMediaType       *pMediaType,
    _In_        DeviceStreamState  reqState
)
{
    UNREFERENCED_PARAMETER(pMediaType);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::ChangeMediaTypeEx");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::ChangeMediaTypeEx pinId: %d", pinId);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::ChangeMediaTypeEx reqState: %d", reqState);


    HRESULT hr = S_OK;
    ComPtr<COutPin> spoPin = GetOutPin(pinId);
    ComPtr<CInPin> spinPin;
    DeviceStreamState       oldOutPinState, oldInputStreamState, newOutStreamState, newRequestedInPinState;
    ComPtr<IMFMediaType>    pFullType, pInputMediaType;
    ULONG                   ulInPinId       = 0;
    DWORD                   dwFlags         = 0;
    

    DMFTCHECKNULL_GOTO(spoPin, done, E_INVALIDARG);
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
           DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, " Pin:%d old MT:[%s] St:%d", pinId, oldType.ToString(), reqState);
           DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, " Pin:%d new MT:[%s] St:%d", pinId, newType.ToString(), reqState);
       }
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
    oldOutPinState      = spoPin->SetState(DeviceStreamState_Disabled);  // Disable output pin
    newOutStreamState = pinStateTransition[oldOutPinState][reqState];  // New state needed  
    
    // The Old input and the output pin states should be the same
    newRequestedInPinState = newOutStreamState;
   
    if ((newOutStreamState != oldOutPinState) /*State change*/
        ||((pFullType.Get() != nullptr) && (pInputMediaType.Get()!=nullptr) && (S_OK != (pFullType->IsEqual(pInputMediaType.Get(), &dwFlags)))) /*Media Types dont match*/
        ||((pFullType == nullptr)||(pInputMediaType == nullptr))/*Either one of the mediatypes is null*/
        )
    {
        //
        // State has change or media type has changed so we need to change the media type on the 
        // underlying kernel pin
        //
        DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "Changing Mediatype on the input ");
        spinPin->setPreferredMediaType(pFullType.Get());
        spinPin->setPreferredStreamState(newRequestedInPinState);
        // Let the pipline know that the input needs to be changed. 
        SendEventToManager(METransformInputStreamStateChanged, GUID_NULL, spinPin->streamId());
        //
        //  The media type will be set on the input pin by the time we return from the wait
        //          
        DMFTCHECKHR_GOTO(spinPin->WaitForSetInputPinMediaChange(), done);
        // Change the media type on the output..
        DMFTCHECKHR_GOTO(spoPin->ChangeMediaTypeFromInpin(pFullType.Get(), pMediaType , reqState), done);
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
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_ERROR, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;

}

//
// The below function sends events to the pipeline.
//

HRESULT CMultipinMft::SendEventToManager(
        _In_ MediaEventType eventType,
        _In_ REFGUID        pGuid,
        _In_ UINT32         context
        )
        /*++
        Description:
        Used to send the event to DTM.
        --*/
    {
    string str = guidToString2(pGuid);

    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SendEventToManager");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SendEventToManager eventType: %d", eventType);
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SendEventToManager pGuid: %s", str.c_str());
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::SendEventToManager context: %d", context);


        HRESULT                 hr      = S_OK;
        ComPtr<IMFMediaEvent>  pEvent  = nullptr;    

        DMFTCHECKHR_GOTO(MFCreateMediaEvent(eventType, pGuid, S_OK, NULL, &pEvent ),done);   
        DMFTCHECKHR_GOTO(pEvent->SetUINT32(MF_EVENT_MFT_INPUT_STREAM_ID, (ULONG)context),done);
        DMFTCHECKHR_GOTO(QueueEvent(pEvent.Get()),done);
    done:
        DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_ERROR, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
         return hr;

    }
/*++
Description:
This function connects the input and output pins.
Any media type filtering can  happen here
--*/
HRESULT CMultipinMft::BridgeInputPinOutputPin(
    _In_ CInPin* piPin,
    _In_ COutPin* poPin
    )
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::BridgeInputPinOutputPin");

    HRESULT hr                      = S_OK;
    ULONG   ulIndex                 = 0;
    ULONG   ulAddedMediaTypeCount   = 0;
    ComPtr<IMFMediaType> spMediaType;

    DMFTCHECKNULL_GOTO( piPin, done, E_INVALIDARG );
    DMFTCHECKNULL_GOTO( poPin, done, E_INVALIDARG );
    //
    // Copy over the media types from input pin to output pin. Since there is no
    // decoder support, only the uncompressed media types are inserted. Please make
    // sure any pin advertised supports at least one media type. The pipeline doesn't
    // like pins with no media types
    //
    while ( SUCCEEDED( hr = piPin->GetMediaTypeAt( ulIndex++, spMediaType.ReleaseAndGetAddressOf() )))
    {
        GUID subType = GUID_NULL;
        DMFTCHECKHR_GOTO( spMediaType->GetGUID(MF_MT_SUBTYPE,&subType), done );
        {
            DMFTCHECKHR_GOTO(hr = poPin->AddMediaType(NULL, spMediaType.Get() ), done );
            if (hr == S_OK)
            {
                ulAddedMediaTypeCount++;
            }
        }
    }
    if (ulAddedMediaTypeCount == 0)
    {
        DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! Make Sure Pin %d has one media type exposed ", piPin->streamId());
        DMFTCHECKHR_GOTO( MF_E_INVALID_STREAM_DATA, done ); 
    }
    //
    //Add the Input Pin to the output Pin
    //
    DMFTCHECKHR_GOTO(poPin->AddPin(piPin->streamId()), done);
    hr = ExceptionBoundary([&](){
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
        DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_ERROR, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    }

    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_ERROR, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;
}


//
// IMFShutdown interface functions
//

/*++
Description:
Implements the Shutdown from IMFShutdown
--*/
STDMETHODIMP CMultipinMft::Shutdown(
    void
    )
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::Shutdown -----");
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::Shutdown E_NOTIMPL -----");
    return E_NOTIMPL;
}

//
// Static method to create an instance of the MFT.
//
HRESULT CMultipinMft::CreateInstance(REFIID iid, void **ppMFT)
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CMultipinMft::CreateInstance -----");

    HRESULT hr = S_OK;
    CMultipinMft *pMFT = NULL;
    DMFTCHECKNULL_GOTO(ppMFT, done, E_POINTER);
    pMFT = new (std::nothrow) CMultipinMft();
    DMFTCHECKNULL_GOTO(pMFT, done, E_OUTOFMEMORY);

    DMFTCHECKHR_GOTO(pMFT->QueryInterface(iid, ppMFT), done);

done:
    if (FAILED(hr))
    {
        SAFERELEASE(pMFT);
    }
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT! SetDefaultAllocator", hr, hr);
    return hr;
}


