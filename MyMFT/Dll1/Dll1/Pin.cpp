#include "pch.h"
#include "Pin.h"
#include "Pin.tmh"

CBasePin::CBasePin(ULONG id, MyMFT* parent)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::CBasePin Constructor -----");

    m_StreamId = id;
    m_Parent = parent;
    m_setMediaType = nullptr;
    m_cRef = 0;
    m_state = DeviceStreamState_Stop;
    m_dwWorkQueueId = MFASYNC_CALLBACK_QUEUE_UNDEFINED;
}

CBasePin::~CBasePin()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::~CBasePin Destructor -----");

    for (ULONG ulIndex = 0, ulSize = (ULONG)m_listOfMediaTypes.size(); ulIndex < ulSize; ulIndex++)
    {
        ComPtr<IMFMediaType> spMediaType;
        spMediaType.Attach(m_listOfMediaTypes[ulIndex]); // Releases the previously stored pointer
    }
    m_listOfMediaTypes.clear();
    m_spAttributes = nullptr;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::QueryInterface(REFIID iid, VOID** ppv)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::QueryInterface -----");

    HRESULT hr = S_OK;

    DMFTCHECKNULL_GOTO(ppv, done, E_POINTER);
    *ppv = nullptr;
    if (iid == __uuidof(IUnknown))
    {
        *ppv = static_cast<VOID*>(this);
    }
    else if (iid == __uuidof(IMFAttributes))
    {
        *ppv = static_cast<IMFAttributes*>(this);
    }
    else if (iid == __uuidof(IKsControl))
    {
        *ppv = static_cast<IKsControl*>(this);
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

STDMETHODIMP_(ULONG __stdcall) CBasePin::AddRef()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::AddRef -----");
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG __stdcall) CBasePin::Release()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::Release -----");

    ULONG count = InterlockedDecrement(&m_cRef);

    if (count == 0)
    {
        delete this;
    }

    return count;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::KsEvent(PKSEVENT pEvent, ULONG ulEventLength, LPVOID pEventData, ULONG ulDataLength, ULONG* pBytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::KsEvent NOP -----");

    UNREFERENCED_PARAMETER(pBytesReturned);
    UNREFERENCED_PARAMETER(ulDataLength);
    UNREFERENCED_PARAMETER(pEventData);
    UNREFERENCED_PARAMETER(pEvent);
    UNREFERENCED_PARAMETER(ulEventLength);
    return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::KsMethod(PKSMETHOD pMethod, ULONG ulMethodLength, LPVOID pMethodData, ULONG ulDataLength, ULONG* pBytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::KsMethod NOP -----");

    UNREFERENCED_PARAMETER(pBytesReturned);
    UNREFERENCED_PARAMETER(ulDataLength);
    UNREFERENCED_PARAMETER(pMethodData);
    UNREFERENCED_PARAMETER(pMethod);
    UNREFERENCED_PARAMETER(ulMethodLength);
    return S_OK;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::KsProperty(PKSPROPERTY pProperty, ULONG ulPropertyLength, LPVOID pPropertyData, ULONG ulDataLength, ULONG* pBytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::KsProperty -----");

    if (m_spIkscontrol != nullptr)
    {
        return m_spIkscontrol->KsProperty(pProperty,
            ulPropertyLength,
            pPropertyData,
            ulDataLength,
            pBytesReturned);
    }
    else
    {
        return E_NOTIMPL;
    }
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::Compare(IMFAttributes*, MF_ATTRIBUTES_MATCH_TYPE, BOOL*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::Compare E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::CompareItem(REFGUID, REFPROPVARIANT, BOOL*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::CompareItem E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::CopyAllItems(IMFAttributes* pDest)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::CopyAllItems -----");
    return m_spAttributes->CopyAllItems(pDest);
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::DeleteAllItems()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::DeleteAllItems E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::DeleteItem(REFGUID guidKey)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::DeleteItem -----");
    return m_spAttributes->DeleteItem(guidKey);
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetAllocatedBlob(REFGUID, UINT8**, UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetAllocatedBlob E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetAllocatedString(REFGUID, LPWSTR*, UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetAllocatedString E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetBlob(REFGUID, UINT8*, UINT32, UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetBlob E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetBlobSize(REFGUID, UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetBlobSize E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetCount(UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetCount E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetDouble(REFGUID, double*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetDouble E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetGUID(REFGUID guidKey, GUID* pguidValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetGUID -----");
    return m_spAttributes->GetGUID(guidKey, pguidValue);
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetItem(REFGUID guidKey, PROPVARIANT* pValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetItem -----");
    return m_spAttributes->GetItem(guidKey, pValue);
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetItemByIndex(UINT32, GUID*, PROPVARIANT*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetItemByIndex E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetItemType(REFGUID, MF_ATTRIBUTE_TYPE*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetItemType E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetString(REFGUID, LPWSTR, UINT32, UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetString E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetStringLength(REFGUID, UINT32*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetStringLength E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetUINT32(REFGUID guidKey, UINT32* punValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetUINT32 -----");

    return m_spAttributes->GetUINT32(guidKey, punValue);
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetUINT64(REFGUID, UINT64*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetUINT64 E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetUnknown(REFGUID guidKey, REFIID riid, LPVOID* ppv)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetUnknown -----");
    return m_spAttributes->GetUnknown(guidKey, riid, ppv);
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::LockStore()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::LockStore E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetBlob(REFGUID, const UINT8*, UINT32)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetBlob E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetDouble(REFGUID, double)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetDouble E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetGUID(REFGUID guidKey, REFGUID guidValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetGUID -----");
    return m_spAttributes->SetGUID(guidKey, guidValue);
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetItem(REFGUID, REFPROPVARIANT)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetItem E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetString(REFGUID guidKey, LPCWSTR wszValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetString -----");

    return m_spAttributes->SetString(guidKey, wszValue);
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetUINT32(REFGUID guidKey, UINT32 unValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetUINT32 -----");
    return m_spAttributes->SetUINT32(guidKey, unValue);
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetUINT64(REFGUID, UINT64)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetUINT64 E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetUnknown(REFGUID, IUnknown*)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetUnknown E_NOTIMPL -----");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::UnlockStore()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::UnlockStore E_NOTIMPL -----");
    return E_NOTIMPL;
}

DeviceStreamState __stdcall CBasePin::SetState(DeviceStreamState State)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetState -----");

    return (DeviceStreamState)InterlockedExchange((LONG*)&m_state, State);
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::AddMediaType(DWORD* pos, IMFMediaType* pMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::AddMediaType Entry");

    UNREFERENCED_PARAMETER(pos);
    UNREFERENCED_PARAMETER(pMediaType);

    HRESULT hr = S_OK;
    CAutoLock Lock(lock());

    DMFTCHECKNULL_GOTO(pMediaType, done, E_INVALIDARG);
    hr = ExceptionBoundary([&]()
        {
            m_listOfMediaTypes.push_back(pMediaType);
        });
    DMFTCHECKHR_GOTO(hr, done);
    pMediaType->AddRef();
    if (pos)
    {
        *pos = (DWORD)(m_listOfMediaTypes.size() - 1);
    }

done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;

}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetMediaTypeAt(DWORD pos, IMFMediaType** ppMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetMediaTypeAt Entry");

    HRESULT hr = S_OK;
    CAutoLock Lock(lock());
    ComPtr<IMFMediaType> spMediaType;
    DMFTCHECKNULL_GOTO(ppMediaType, done, E_INVALIDARG);
    *ppMediaType = nullptr;
    if (pos >= m_listOfMediaTypes.size())
    {
        DMFTCHECKHR_GOTO(MF_E_NO_MORE_TYPES, done);
    }
    spMediaType = m_listOfMediaTypes[pos];
    *ppMediaType = spMediaType.Detach();
done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetOutputAvailableType(DWORD dwTypeIndex, IMFMediaType** ppType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetOutputAvailableType");

    return GetMediaTypeAt(dwTypeIndex, ppType);
}

STDMETHODIMP_(BOOL __stdcall) CBasePin::IsMediaTypeSupported(IMFMediaType* pMediaType, IMFMediaType** ppIMFMediaTypeFull)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::IsMediaTypeSupported");

    HRESULT hr = S_OK;
    BOOL bFound = FALSE;
    CAutoLock Lock(lock());
    DMFTCHECKNULL_GOTO(pMediaType, done, E_INVALIDARG);
    if (ppIMFMediaTypeFull)
    {
        *ppIMFMediaTypeFull = nullptr;
    }

    for (UINT uIIndex = 0, uISize = (UINT)m_listOfMediaTypes.size(); uIIndex < uISize; uIIndex++)
    {
        DWORD dwResult = 0;
        hr = m_listOfMediaTypes[uIIndex]->IsEqual(pMediaType, &dwResult);
        if (hr == S_FALSE)
        {

            if ((dwResult & MF_MEDIATYPE_EQUAL_MAJOR_TYPES) &&
                (dwResult & MF_MEDIATYPE_EQUAL_FORMAT_TYPES) &&
                (dwResult & MF_MEDIATYPE_EQUAL_FORMAT_DATA))
            {
                hr = S_OK;
            }
        }
        if (hr == S_OK)
        {
            bFound = TRUE;
            if (ppIMFMediaTypeFull) {
                *ppIMFMediaTypeFull = m_listOfMediaTypes[uIIndex];
                (*ppIMFMediaTypeFull)->AddRef();
            }
            break;
        }
        else if (FAILED(hr))
        {
            DMFTCHECKHR_GOTO(hr, done);
        }
    }
done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return SUCCEEDED(hr) ? TRUE : FALSE;
}

CInPin::CInPin(IMFAttributes* pAttributes, ULONG ulPinId, MyMFT* pParent) : CBasePin(ulPinId, pParent)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::CInPin Constructor -----");

    m_stStreamType = GUID_NULL;
    m_waitInputMediaTypeWaiter = NULL;
    m_preferredStreamState = DeviceStreamState_Stop;

    setAttributes(pAttributes);
}

CInPin::~CInPin()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::~CInPin Destructor -----");

    setAttributes(nullptr);
    m_spSourceTransform = nullptr;

    if (m_waitInputMediaTypeWaiter)
    {
        CloseHandle(m_waitInputMediaTypeWaiter);
    }
}

STDMETHODIMP_(HRESULT __stdcall) CInPin::Init(IMFTransform* pTransform)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::Init Entry");

    HRESULT hr = S_OK;

    DMFTCHECKNULL_GOTO(pTransform, done, E_INVALIDARG);

    m_spSourceTransform = pTransform;

    DMFTCHECKHR_GOTO(GetGUID(MF_DEVICESTREAM_STREAM_CATEGORY, &m_stStreamType), done);

    //
    //Get the DevProxy IKSControl.. used to send the KSControls or the device control IOCTLS over to devproxy and finally on to the driver!!!!
    //
    DMFTCHECKHR_GOTO(m_spAttributes.As(&m_spIkscontrol), done);

    m_waitInputMediaTypeWaiter = CreateEvent(NULL,
        FALSE,
        FALSE,
        TEXT("MediaTypeWaiter")
    );
    DMFTCHECKNULL_GOTO(m_waitInputMediaTypeWaiter, done, E_OUTOFMEMORY);

    DMFTCHECKHR_GOTO(GenerateMFMediaTypeListFromDevice(streamId()), done);

done:
    if (FAILED(hr))
    {
        m_spSourceTransform = nullptr;

        if (m_waitInputMediaTypeWaiter)
        {
            CloseHandle(m_waitInputMediaTypeWaiter);
            m_waitInputMediaTypeWaiter = NULL;
        }

        m_stStreamType = GUID_NULL;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;
}

HRESULT CInPin::GenerateMFMediaTypeListFromDevice(UINT uiStreamId)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::GenerateMFMediaTypeListFromDevice Entry");

    HRESULT hr = S_OK;
    GUID stSubType = { 0 };
    //This is only called in the begining when the input pin is constructed
    DMFTCHECKNULL_GOTO(m_spSourceTransform, done, MF_E_TRANSFORM_TYPE_NOT_SET);
    for (UINT iMediaType = 0; SUCCEEDED(hr); iMediaType++)
    {
        ComPtr<IMFMediaType> spMediaType;
        DWORD pos = 0;

        //hr = m_spSourceTransform->MFTGetOutputAvailableType(uiStreamId, iMediaType, spMediaType.GetAddressOf());
        hr = m_spSourceTransform->GetOutputAvailableType(uiStreamId, iMediaType, spMediaType.GetAddressOf());
        if (hr != S_OK)
            break;

        DMFTCHECKHR_GOTO(AddMediaType(&pos, spMediaType.Get()), done);
    }
done:
    if (hr == MF_E_NO_MORE_TYPES) {
        hr = S_OK;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;
}

STDMETHODIMP_(VOID __stdcall) CInPin::ConnectPin(CBasePin* poPin)
{
    UNREFERENCED_PARAMETER(poPin);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::ConnectPin");
}

STDMETHODIMP_(HRESULT __stdcall) CInPin::WaitForSetInputPinMediaChange()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::WaitForSetInputPinMediaChange Entry");

    DWORD   dwWait = 0;
    HRESULT hr = S_OK;

    dwWait = WaitForSingleObject(m_waitInputMediaTypeWaiter, INFINITE);

    if (dwWait != WAIT_OBJECT_0)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        goto done;
    }
done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;

}

HRESULT CInPin::GetInputStreamPreferredState(DeviceStreamState* value, IMFMediaType** ppMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::GetInputStreamPreferredState Entry");

    HRESULT hr = S_OK;
    CAutoLock Lock(lock());

    if (value != nullptr)
    {
        *value = m_preferredStreamState;
    }

    if (ppMediaType)
    {
        *ppMediaType = nullptr;
        if (m_spPrefferedMediaType != nullptr)
        {
            m_spPrefferedMediaType.CopyTo(ppMediaType);
        }
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;
}

HRESULT CInPin::SetInputStreamState(IMFMediaType* pMediaType, DeviceStreamState value, DWORD dwFlags)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::SetInputStreamState Entry");

    HRESULT hr = S_OK;
    UNREFERENCED_PARAMETER(dwFlags);

    CAutoLock Lock(lock());
    //
    //Set the media type
    //

    setMediaType(pMediaType);
    SetState(value);

    //
    //Set the event. This event is being waited by an output media/state change operation
    //

    m_spPrefferedMediaType = nullptr;
    SetEvent(m_waitInputMediaTypeWaiter);

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;
}

COutPin::COutPin(ULONG ulPinId, MyMFT* pparent, IKsControl* pIksControl, MFSampleAllocatorUsage allocatorUsage) : CBasePin(ulPinId, pparent)
    , m_firstSample(false)
    , m_allocatorUsage(allocatorUsage)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "COutPin::COutPin Constructor");

    HRESULT                 hr = S_OK;
    ComPtr<IMFAttributes>   spAttributes;

    //
    //Get the input pin IKS control.. the pin IKS control talks to sourcetransform's IKS control
    //
    m_spIkscontrol = pIksControl;

    MFCreateAttributes(&spAttributes, 3); //Create the space for the attribute store!!
    setAttributes(spAttributes.Get());
    DMFTCHECKHR_GOTO(SetUINT32(MFT_SUPPORT_DYNAMIC_FORMAT_CHANGE, TRUE), done);
    DMFTCHECKHR_GOTO(SetString(MFT_ENUM_HARDWARE_URL_Attribute, L"Sample_CameraExtensionMft"), done);
    DMFTCHECKHR_GOTO(SetUINT32(MF_TRANSFORM_ASYNC, TRUE), done);

done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
}

STDMETHODIMP_(HRESULT __stdcall) COutPin::AddPin(DWORD)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "COutPin::AddPin Entry");

    HRESULT hr = S_OK;
    CAutoLock Lock(lock());

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return S_OK;
}

STDMETHODIMP_(VOID __stdcall) COutPin::SetFirstSample(BOOL)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "COutPin::SetFirstSample NOP");
    return VOID();
}

STDMETHODIMP_(HRESULT __stdcall) COutPin::ChangeMediaTypeFromInpin(IMFMediaType* pInMediatype, IMFMediaType* pOutMediaType, DeviceStreamState state)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "COutPin::ChangeMediaTypeFromInpin Entry");

    HRESULT hr = S_OK;
    CAutoLock Lock(lock());
    //
    //Set the state to disabled and while going out we will reset the state back to the requested state
    //Flush so that we drop any samples we have in store!!
    //
    SetState(DeviceStreamState_Disabled);
    pInMediatype;
    hr = S_OK;
    if (SUCCEEDED(hr))
    {
        (VOID)setMediaType(pOutMediaType);
        (VOID)SetState(state);
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);
    return hr;
}
