//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media Foundation
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
//
#pragma once
#include "stdafx.h"
#include "common.h"
#include <deque>
using namespace std;
#include <functional>

typedef void(*DMFT_IMAGE_TRANSFORM_FN)(
    const RECT&             rcDest,          // Destination rectangle for the transformation.
    BYTE*                   pDest,           // Destination buffer.
    LONG                    lDestStride,     // Destination stride.
    const BYTE*             pSrc,            // Source buffer.
    LONG                    lSrcStride,      // Source stride.
    DWORD                   dwWidthInPixels, // Image width in pixels.
    DWORD                   dwHeightInPixels // Image height in pixels.
    );
//
//Queue class!!!
//
class Ctee;
//typedef CMFAttributesTrace CMediaTypeTrace; /* Only used for debug. take this out*/


template< typename T ,  HRESULT ( __stdcall T::*Func) ( IMFAsyncResult* ) >
class CDMFTAsyncCallback : public IMFAsyncCallback
{
public:
    CDMFTAsyncCallback( T* parent , DWORD dwWorkQueueId = MFASYNC_CALLBACK_QUEUE_STANDARD) :
        m_Parent(parent),
        m_dwQueueId(dwWorkQueueId)
    { }
    virtual ~CDMFTAsyncCallback() { }

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        HRESULT hr = S_OK;
        if (ppv != nullptr)
        {
            *ppv = nullptr;
            if (riid == __uuidof(IMFAsyncCallback) || riid == __uuidof(IUnknown))
            {
                AddRef();
                *ppv = static_cast<IUnknown*>(this);
            }
            else
            {
                hr = E_NOINTERFACE;
            }
        }
        else
        {
            hr = E_POINTER;
        }
        return hr;
    }

    STDMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }
    STDMETHODIMP_(ULONG) Release()
    {
        long cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0)
        {
            delete this;
        }
        return cRef;
    }

    STDMETHODIMP GetParameters(DWORD* pdwFlags, DWORD* pdwQueue)
    {
        *pdwFlags = 0;
        *pdwQueue = m_dwQueueId;
        return E_NOTIMPL;
    }

    STDMETHODIMP Invoke( IMFAsyncResult* pAsyncResult )
    {
        ComPtr<T> spParent;
        {
            // Take a reference on the parent so that
            // shutdown may not yank it from us
            CAutoLock Lock(&m_Lock);
            spParent = m_Parent;
        }
        if (spParent.Get())
        {
            return ((spParent.Get())->*Func)(pAsyncResult);
        }
        return MF_E_SHUTDOWN;
    }
    VOID Shutdown()
    {
        CAutoLock Lock(&m_Lock);
        m_Parent = nullptr; //Break the reference
    }
    T GetParent()
    {
        return m_Parent;
    }
protected:
    CCritSec    m_Lock;
    ComPtr<T>   m_Parent; 
    long        m_cRef = 0;
    DWORD       m_dwQueueId = MFASYNC_CALLBACK_QUEUE_STANDARD;
};



//
// Define these in different components
// The below classes are used to add the
// XVP and the Decoder components.
//
class Ctee: public IUnknown{
public:
    // This is a helper class to release the interface
    // It will first call shutdowntee to break any circular
    // references any components might have with their composed
    // objects
    static VOID ReleaseTee( _In_ ComPtr<Ctee> &tee)
    {
        if (tee)
        {
            tee->ShutdownTee();
            tee = nullptr;
        }
    }
    STDMETHOD(Start)()
    {
        return S_OK;
    }
    STDMETHOD(Stop)()
    {
        return S_OK;
    }

   STDMETHOD_(VOID, ShutdownTee)()
   {
       return; // NOOP
   }
   STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
   {
       HRESULT hr = S_OK;
       if (ppv != nullptr)
       {
           *ppv = nullptr;
           if (riid == __uuidof(IUnknown))
           {
               AddRef();
               *ppv = static_cast<IUnknown*>(this);
           }
           else
           {
               hr = E_NOINTERFACE;
           }
       }
       else
       {
           hr = E_POINTER;
       }
       return hr;
   }
   Ctee()
   {
   }
   virtual ~Ctee()
   {}
   STDMETHODIMP_(ULONG) AddRef()
   {
       return InterlockedIncrement(&m_cRef);
   }
   STDMETHODIMP_(ULONG) Release()
   {
       long cRef = InterlockedDecrement(&m_cRef);
       if (cRef == 0)
       {
           delete this;
       }
       return cRef;
   }

protected:
    ULONG m_cRef = 0;
};


class CWrapTee : public Ctee
{
public:
    CWrapTee( _In_ Ctee *tee=nullptr ) 
    : m_spObjectWrapped(tee)
    , m_pInputMediaType(nullptr)
    , m_pOutputMediaType(nullptr)
    {

    }
    virtual ~CWrapTee()
    {
    }

    //
    // Inline functions
    //
protected:
    
    __inline IMFMediaType* getInMediaType()
    {
        IMFMediaType* pmediaType = nullptr;
        m_pInputMediaType.CopyTo(&pmediaType);
        return pmediaType;
    }
    __inline IMFMediaType* getOutMediaType()
    {
        IMFMediaType* pmediaType = nullptr;
        m_pOutputMediaType.CopyTo(&pmediaType);
        return pmediaType;
    }

protected:
    
    ComPtr< IMFMediaType > m_pInputMediaType;
    ComPtr< IMFMediaType > m_pOutputMediaType;
    ComPtr<Ctee> m_spObjectWrapped;
};

//
// wrapper class for encoder and decoder
//
class CVideoProcTee: public CWrapTee
{
public:

    CVideoProcTee( _In_ Ctee* p,  _In_ GUID category = PINNAME_PREVIEW
        , _In_ IMFVideoSampleAllocator* sampleAllocator=nullptr
    )
        :CWrapTee(p)
        , m_bProducesSamples(FALSE)
        , m_asyncHresult(S_OK)
        , m_streamCategory(category)
        , m_fSetD3DManager(FALSE)
        , m_spDefaultAllocator(sampleAllocator)
    {}
    __inline IMFTransform* Transform()
    {
        return m_spVideoProcessor.Get();
    }

    VOID SetD3DManager( _In_opt_ IUnknown* pUnk )
    {
        m_spDeviceManagerUnk = pUnk;
    }

    virtual ~CVideoProcTee()
    {}
protected:
    CCritSec m_Lock;
    __inline VOID SetAsyncStatus(_In_ HRESULT hrStatus)
    {
        InterlockedCompareExchange(&m_asyncHresult, hrStatus, S_OK);
    }
    HRESULT GetAsyncStatus()
    {
        return InterlockedCompareExchange(&m_asyncHresult, S_OK, S_OK);
    }
    HRESULT              m_asyncHresult;
    ComPtr< IMFTransform > m_spVideoProcessor;
    ComPtr<IUnknown>       m_spDeviceManagerUnk;
    ComPtr<IMFVideoSampleAllocatorEx> m_spPrivateAllocator;
    ComPtr<IMFVideoSampleAllocator> m_spDefaultAllocator;

    BOOL                   m_bProducesSamples;
    GUID                   m_streamCategory;
    BOOL                   m_fSetD3DManager;
};

class CXvptee :public CVideoProcTee{
public:
    CXvptee( _In_ Ctee *, _In_ GUID category = PINNAME_PREVIEW );
    virtual ~CXvptee();

};

class CDecoderTee : public CVideoProcTee {
public:

    CDecoderTee(_In_ Ctee *t,_In_ DWORD dwQueueId,GUID category=PINNAME_PREVIEW) :
        CVideoProcTee(t)
        , m_fAsyncMFT(FALSE)
        , m_D3daware(FALSE)
        , m_hwMFT(FALSE)
        , m_asyncHresult(S_OK)
        , m_lNeedInputRequest(0)
        , m_streamCategory(category)
        , m_bXvpAdded(FALSE)
        , m_dwMFTInputId(0)
        , m_dwMFTOutputId(0)
        , m_dwQueueId(dwQueueId)
        , m_dwCameraStreamWorkQueueId(0)
    {
        m_streamCategory = category;
    }
    virtual ~CDecoderTee();

protected:
    STDMETHOD_(VOID, ShutdownTee)();

    BOOL                 m_fAsyncMFT;
    BOOL                 m_D3daware;
    BOOL                 m_hwMFT;
    HRESULT              m_asyncHresult;
    DWORD                m_lNeedInputRequest;
    GUID                 m_streamCategory; // Needed for bind flags
    BOOL                 m_bXvpAdded;
    DWORD                m_dwMFTInputId;
    DWORD                m_dwMFTOutputId;
    ComPtr<IMFSample>    m_spUnprocessedSample;
    DWORD                m_dwQueueId;
    DWORD                m_dwCameraStreamWorkQueueId;
    std::deque<ComPtr<IMFSample> > m_InputSampleList;

};

class CSampleCopytee :public CVideoProcTee {
public:
    CSampleCopytee(_In_ Ctee*, _In_ GUID category = PINNAME_PREVIEW
        , _In_ IMFVideoSampleAllocator* sampleAllocator = nullptr
    );
    ~CSampleCopytee();
};

#ifdef MF_DEVICEMFT_ADD_GRAYSCALER_
class CGrayTee : public CWrapTee {
public:
    CGrayTee(Ctee*);
    ~CGrayTee() {
    }

    STDMETHODIMP Do(_In_ IMFSample* pSample, _Out_ IMFSample **, , _Inout_ bool &newSample);
    STDMETHODIMP Configure(_In_opt_ IMFMediaType *, _In_opt_ IMFMediaType *, _Outptr_ IMFTransform**);
private:
    // Function pointer for the function that transforms the image.
    DMFT_IMAGE_TRANSFORM_FN m_transformfn;
    RECT m_rect;
};
#endif
/*
################## EVENT HANDLING #############################################
Events are usually divided into two categories by the Capture Pipeline
1) Regular Event / Manual Reset Event
2) One Shot Event

Regular events are set and cleared by the pipeline
One shot Events will only be set by the pipeline and it should be cleared
by the Component managing the Event Store i.e. KS or DMFT. For Redstone
The pipeline should not send any Non One shot events.This sample however
does show the handling of Regular/ Manual Reset events

This clearing of One shot events is done when the event is fired..
E.g before sending the warm start command The pipeline will send a one shot event
KSPROPERTY_CAMERACONTROL_EXTENDED_WARMSTART and when the operation completes the
event should be fired.
The Device MFT should usually keep a watch for one shot events sent by the Pipeline
for Async Extended controls..The list of Asynchronous controls are as follows..

KSPROPERTY_CAMERACONTROL_EXTENDED_PHOTOMODE
KSPROPERTY_CAMERACONTROL_EXTENDED_PHOTOMAXFRAMERATE
KSPROPERTY_CAMERACONTROL_EXTENDED_FOCUSMODE
KSPROPERTY_CAMERACONTROL_REGION_OF_INTEREST_PROPERTY_ID
KSPROPERTY_CAMERACONTROL_EXTENDED_ISO
KSPROPERTY_CAMERACONTROL_EXTENDED_ISO_ADVANCED
KSPROPERTY_CAMERACONTROL_EXTENDED_EVCOMPENSATION
KSPROPERTY_CAMERACONTROL_EXTENDED_WHITEBALANCEMODE
KSPROPERTY_CAMERACONTROL_EXTENDED_EXPOSUREMODE
KSPROPERTY_CAMERACONTROL_EXTENDED_SCENEMODE
KSPROPERTY_CAMERACONTROL_EXTENDED_PHOTOTHUMBNAIL
KSPROPERTY_CAMERACONTROL_EXTENDED_WARMSTART
KSPROPERTY_CAMERACONTROL_EXTENDED_ROI_ISPCONTROL
KSPROPERTY_CAMERACONTROL_EXTENDED_PROFILE
The complete list can be found from the Camera DDI spec
###############################################################################
*/

typedef struct _DMFTEventEntry{
    ULONG   m_ulEventId;        // KSEVENT->Id
    PVOID   m_pEventData;       // Lookup for events in the data structure
    HANDLE  m_hHandle;          // The duplicate handle stored from the event
    //
    // Constructor. We simply cache the handles, the property id and the KSEVENTDATA buffer sent from the user mode
    //
    _DMFTEventEntry( _In_ ULONG ulEventId, _In_ PVOID pEventData, _In_ HANDLE pHandle):m_pEventData(pEventData)
        , m_ulEventId(ulEventId)
        , m_hHandle(pHandle)
    {
    }
    ~_DMFTEventEntry()
    {
        if ( m_hHandle != nullptr )
        {
            CloseHandle(m_hHandle);
            m_hHandle = nullptr;
        }
    }

}DMFTEventEntry, *PDMFTEventEntry;

//
// Handler for one shot events and Normal events
//

class CMultipinMft;
class CInPin;
class COutPin;

class CPinCreationFactory {
protected:
    ComPtr<CMultipinMft> m_spDeviceTransform;
public:
    typedef enum _type_pin {
        DMFT_PIN_INPUT,
        DMFT_PIN_OUTPUT,
        DMFT_PIN_CUSTOM,
        DMFT_PIN_ALLOCATOR_PIN,
        DMFT_MAX
    }type_pin;
    HRESULT CreatePin( _In_ ULONG ulInputStreamId, _In_ ULONG ulOutStreamId, _In_ type_pin type,_Outptr_ CBasePin** ppPin, _In_ BOOL& isCustom);
    CPinCreationFactory(_In_ CMultipinMft* pDeviceTransform):m_spDeviceTransform(pDeviceTransform){
    }
};


