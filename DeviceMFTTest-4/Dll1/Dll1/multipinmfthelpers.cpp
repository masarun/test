#include "pch.h"
#include "stdafx.h"
#include "common.h"
#include "multipinmfthelpers.h"
#include "multipinmft.h"
#include "basepin.h"
#include <wincodec.h>
#include <Codecapi.h>

#ifdef MF_WPP
#include "multipinmfthelpers.tmh"    //--REF_ANALYZER_DONT_REMOVE--
#endif

class CMediaTypePrinter;


// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// @@@@ README: Video Processor functions below
//
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//

CXvptee::CXvptee(_In_ Ctee *tee, GUID category) :
    CVideoProcTee(tee, category)
{

}

CXvptee::~CXvptee()
{
    m_spDeviceManagerUnk = nullptr;
}




// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// @@@@ README: Decoder related functions below
//
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//

CDecoderTee::~CDecoderTee()
{
    MFUnlockWorkQueue(m_dwCameraStreamWorkQueueId);
    if (m_spPrivateAllocator)
    {
        m_spPrivateAllocator->UninitializeSampleAllocator();
        m_spPrivateAllocator = nullptr;
    }
}



VOID CDecoderTee::ShutdownTee()
{
    CAutoLock lock(m_Lock);
    SetAsyncStatus(MF_E_SHUTDOWN);
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
// @@@@ README: Sample copy related functions below
//
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//

CSampleCopytee::CSampleCopytee(_In_ Ctee *tee, GUID category
    , IMFVideoSampleAllocator* sampleAllocator
) :
    CVideoProcTee(tee, category
        , sampleAllocator
    )
{

}

CSampleCopytee::~CSampleCopytee()
{
    m_spDeviceManagerUnk = nullptr;
}



HRESULT CPinCreationFactory::CreatePin(_In_ ULONG ulInputStreamId, /* The Input stream Id*/
    _In_ ULONG ulOutStreamId, /*The output stream Id*/
    _In_ type_pin type, /*Input Pin or the ourput pin*/
    _Outptr_ CBasePin** ppPin, /*Output*/
    _In_ BOOL& isCustom)
{
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "CPinCreationFactory::CreatePin");


    HRESULT hr = S_OK;
    ComPtr<IMFAttributes> spAttributes;
    GUID    streamCategory = GUID_NULL;
    DMFTCHECKNULL_GOTO(ppPin, done, E_INVALIDARG);
    *ppPin = nullptr;
    isCustom = FALSE;
    DMFTCHECKHR_GOTO(m_spDeviceTransform->Parent()->GetOutputStreamAttributes(ulInputStreamId, &spAttributes),done);
    if (type == DMFT_PIN_INPUT)
    {
        ComPtr<CInPin>  spInPin;
        DMFTCHECKHR_GOTO(spAttributes->GetGUID(MF_DEVICESTREAM_STREAM_CATEGORY, &streamCategory), done);
        // Create Cutom Pin
        if (IsEqualCLSID(streamCategory, AVSTREAM_CUSTOM_PIN_IMAGE))
        {
            isCustom = TRUE;
        }
        else
        {
            spInPin = new (std::nothrow) CAsyncInPin(spAttributes.Get(), ulInputStreamId, m_spDeviceTransform.Get()); // Asynchronous PIn, if you need it
        }
        DMFTCHECKNULL_GOTO(spInPin.Get(), done, E_OUTOFMEMORY);
        *ppPin = spInPin.Detach();

    }
    else if(type == DMFT_PIN_OUTPUT)
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

        spOutPin = new (std::nothrow) COutPin(ulOutStreamId, m_spDeviceTransform.Get(), spKscontrol.Get()
            , MFSampleAllocatorUsage_DoesNotAllocate
        );         // Create the output pin
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
            DMFTCHECKHR_GOTO(spOutPin->SetUINT32(MF_DEVICESTREAM_ATTRIBUTE_FRAMESOURCE_TYPES, uiFrameSourceType),done);   // Copy over the Frame Source Type.
        }

#if defined (MF_DEVICEMFT_ALLOW_MFT0_LOAD) && defined (MFT_UNIQUE_METHOD_NAMES)
        //
        // If we wish to load MFT0 as well as Device MFT then we should be doing the following
        // Copy over the GUID attribute MF_DEVICESTREAM_EXTENSION_PLUGIN_CLSID from the input
        // pin to the output pin. This is because Device MFT is the new face of the filter now
        // and MFT0 will now get loaded for the output pins exposed from Device MFT rather than
        // DevProxy!
        //

        GUID        guidMFT0 = GUID_NULL;
        if (SUCCEEDED(spInPin->GetGUID(MF_DEVICESTREAM_EXTENSION_PLUGIN_CLSID, &guidMFT0)))
        {
            //
            // This stream has an MFT0 .. Attach the GUID to the Outpin pin attribute
            // The downstream will query this attribute  on the pins exposed from device MFT
            //
            DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! setting Mft0 guid on pin %d", ulOutStreamId);

            DMFTCHECKHR_GOTO(spOutPin->SetGUID(MF_DEVICESTREAM_EXTENSION_PLUGIN_CLSID, guidMFT0), done);

            DMFTCHECKHR_GOTO(spOutPin->SetUnknown(MF_DEVICESTREAM_EXTENSION_PLUGIN_CONNECTION_POINT,
                static_cast< IUnknown* >(static_cast < IKsControl * >(m_spDeviceTransform.Get()))), done);

        }
#endif
        *ppPin = spOutPin.Detach();
    }
    else
    {
        DMFTCHECKHR_GOTO(E_INVALIDARG, done);
    }

done:
    DMFTRACE(DMFT_GENERAL, TRACE_LEVEL_INFORMATION, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;
}

