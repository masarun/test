#include "pch.h"
#include "CBasePin.h"
#include "CBasePin.tmh"

#include <string>
#include <array>
using namespace std;

CBasePin::CBasePin(DWORD streamId)
{
    m_cRef = 0;
    m_streamId = streamId;
    m_Parent = NULL;
}


// https://stackoverflow.com/questions/21431047/how-to-convert-guid-to-char?rq=1
std::string guidToString2(GUID guid) {
    std::array<char, 40> output;
    snprintf(output.data(), output.size(), "{%08X-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X}", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    return std::string(output.data());
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::QueryInterface(REFIID rIID, VOID** ppInterface)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::QueryInterface Entry");

    HRESULT hr = S_OK;

    if (rIID == __uuidof(IUnknown))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::QueryInterface");
        *ppInterface = static_cast<VOID *>(this);
    }
    else if (rIID == __uuidof(IMFAttributes))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::QueryInterface IMFAttributes");
        *ppInterface = static_cast<IMFAttributes*>(this);
    }
    else if (rIID == __uuidof(IKsControl))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::QueryInterface IKsControl");
        *ppInterface = static_cast<IKsControl*>(this);
    }
    else
    {
        string strGuid = guidToString2(rIID);
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::QueryInterface Entry E_NOINTERFACE %s", strGuid.c_str());

        *ppInterface = NULL;

        return E_NOINTERFACE;
    }

    ((LPUNKNOWN)*ppInterface)->AddRef();

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::QueryInterface Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(ULONG __stdcall) CBasePin::AddRef()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::AddRef");
    return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG __stdcall) CBasePin::Release()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::Release");

    InterlockedDecrement(&m_cRef);

    if (m_cRef)
    {
        return m_cRef;
    }

    delete this;

    return 0;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::KsEvent(PKSEVENT Event, ULONG EventLength, LPVOID EventData, ULONG DataLength, ULONG* BytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::KsEvent E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::KsMethod(PKSMETHOD Method, ULONG MethodLength, LPVOID MethodData, ULONG DataLength, ULONG* BytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::KsMethod E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::KsProperty(PKSPROPERTY Property, ULONG PropertyLength, LPVOID PropertyData, ULONG DataLength, ULONG* BytesReturned)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::KsProperty E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::Compare(IMFAttributes* pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL* pbResult)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::Compare E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::CompareItem(REFGUID guidKey, REFPROPVARIANT Value, BOOL* pbResult)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::CompareItem E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::CopyAllItems(IMFAttributes* pDest)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::CopyAllItems E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::DeleteAllItems()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::DeleteAllItems E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::DeleteItem(REFGUID guidKey)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::DeleteItem E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetAllocatedBlob(REFGUID guidKey, UINT8** ppBuf, UINT32* pcbSize)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetAllocatedBlob E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetAllocatedString(REFGUID guidKey, LPWSTR* ppwszValue, UINT32* pcchLength)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetAllocatedString E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetBlob(REFGUID guidKey, UINT8* pBuf, UINT32 cbBufSize, UINT32* pcbBlobSize)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetBlob E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetBlobSize(REFGUID guidKey, UINT32* pcbBlobSize)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetBlobSize E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetCount(UINT32* pcItems)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetCount E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetDouble(REFGUID guidKey, double* pfValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetDouble E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetGUID(REFGUID guidKey, GUID* pguidValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetGUID Entry");

    HRESULT hr = E_FAIL;

    try
    { 
        hr = m_spAttributes->GetGUID(guidKey, pguidValue);

        string strGuid2 = guidToString2(guidKey);
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetGUID guidKey: %s", strGuid2.c_str());

        string strGuid = guidToString2(*pguidValue);
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetGUID GUID: %s", strGuid.c_str());
    }
    catch (...)
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetGUID Exception");
    }

    /*
    if (SUCCEEDED(hr))
    {
        string strGuid = guidToString2(*pguidValue);
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetGUID GUID: %s", strGuid.c_str());
    }
    */
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetGUID Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetItem(REFGUID guidKey, PROPVARIANT* pValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetItem E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetItemByIndex(UINT32 unIndex, GUID* pguidKey, PROPVARIANT* pValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetItemByIndex E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetItemType(REFGUID guidKey, MF_ATTRIBUTE_TYPE* pType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetItemType E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetString(REFGUID guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32* pcchLength)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetString E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetStringLength(REFGUID guidKey, UINT32* pcchLength)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetStringLength E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetUINT32(REFGUID guidKey, UINT32* punValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetUINT32 Entry");

    string strGuid = guidToString2(guidKey);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetGUID guidKey: %s", strGuid.c_str());

    HRESULT hr = m_spAttributes->GetUINT32(guidKey, punValue);

    if (SUCCEEDED(hr))
    {
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetGUID Value: %d", *punValue);
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetUINT32 Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetUINT64(REFGUID guidKey, UINT64* punValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetUINT64 E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::GetUnknown(REFGUID guidKey, REFIID riid, LPVOID* ppv)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetUnknown E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::LockStore()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::LockStore E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetBlob(REFGUID guidKey, const UINT8* pBuf, UINT32 cbBufSize)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetBlob E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetDouble(REFGUID guidKey, double fValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetDouble E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetGUID(REFGUID guidKey, REFGUID guidValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetGUID Entry");

    string strGuid2 = guidToString2(guidKey);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetGUID guidKey: %s", strGuid2.c_str());

    string strGuid = guidToString2(guidValue);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetGUID guidValue: %s", strGuid.c_str());

    HRESULT hr = m_spAttributes->SetGUID(guidKey, guidValue);

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetGUID Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetItem(REFGUID guidKey, REFPROPVARIANT Value)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetItem E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetString(REFGUID guidKey, LPCWSTR wszValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetString Entry");

    string strGuid2 = guidToString2(guidKey);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetString guidKey: %s", strGuid2.c_str());

    HRESULT hr = m_spAttributes->SetString(guidKey, wszValue);
    
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetString Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetUINT32(REFGUID guidKey, UINT32 unValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetUINT32 Entry");

    string strGuid2 = guidToString2(guidKey);
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetUINT32 guidKey: %s", strGuid2.c_str());

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetUINT32 unValue: %d", unValue);

    HRESULT hr = m_spAttributes->SetUINT32(guidKey, unValue);

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetUINT32 Exit %!HRESULT!", hr);
    return hr;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetUINT64(REFGUID guidKey, UINT64 unValue)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetUINT64 E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::SetUnknown(REFGUID guidKey, IUnknown* pUnknown)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::SetUnknown E_NOTIMPL");
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CBasePin::UnlockStore()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::UnlockStore E_NOTIMPL");
    return E_NOTIMPL;
}

DWORD CBasePin::GetStreamId()
{
    return m_streamId;
}

CInPin::CInPin(IMFAttributes* pAttributes, DWORD inputStreamId, MyMFT* parent) : CBasePin(inputStreamId, parent)
{
    m_spAttributes = nullptr;
    m_spSourceTransform = nullptr;
    m_waitInputMediaTypeWaiter = NULL;

    m_spAttributes = pAttributes;
}

CInPin::~CInPin()
{
    if (m_waitInputMediaTypeWaiter)
    {
        CloseHandle(m_waitInputMediaTypeWaiter);
        m_waitInputMediaTypeWaiter = NULL;
    }
}

STDMETHODIMP_(HRESULT __stdcall) CInPin::Init(IMFTransform* pTransform)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::Init Entry");
    HRESULT hr = E_FAIL;

    if (pTransform != NULL)
    {
        m_spSourceTransform = pTransform;

        hr = GetGUID(MF_DEVICESTREAM_STREAM_CATEGORY, &m_stStreamType);
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::Init GetGUID %!HRESULT!", hr);

        hr = m_spAttributes.As(&m_spIkscontrol);
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::Init m_spAttributes.As %!HRESULT!", hr);

        m_waitInputMediaTypeWaiter = CreateEvent(NULL,
            FALSE,
            FALSE,
            TEXT("MediaTypeWaiter")
        );
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::Init CreateEvent");

        hr = GenerateMFMediaTypeListFromDevice(GetStreamId());
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::Init GenerateMFMediaTypeListFromDevice %!HRESULT!", hr);
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::Init Exit %!HRESULT!", hr);
    return hr;
}

HRESULT CInPin::GenerateMFMediaTypeListFromDevice(UINT uiStreamId)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::GenerateMFMediaTypeListFromDevice Entry");

    HRESULT hr = S_OK;

    if (m_spSourceTransform != NULL)
    {
        for (DWORD index = 0; SUCCEEDED(hr); index++)
        {
            ComPtr<IMFMediaType> spMediaType;
            DWORD pos = 0;

            hr = m_spSourceTransform->GetOutputAvailableType(uiStreamId, index, spMediaType.GetAddressOf());
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::GenerateMFMediaTypeListFromDevice m_spSourceTransform->GetOutputAvailableType %!HRESULT!", hr);

            if (hr != S_OK)
            {
                break;
            }

            hr = AddMediaType(&pos, spMediaType.Get());
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::GenerateMFMediaTypeListFromDevice AddMediaType %!HRESULT!", hr);
        }

        if (hr == MF_E_NO_MORE_TYPES)
        {
            TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::GenerateMFMediaTypeListFromDevice MF_E_NO_MORE_TYPES");
            hr = S_OK;
        }

    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::GenerateMFMediaTypeListFromDevice Exit %!HRESULT!", hr);
    return hr;
}

HRESULT CBasePin::AddMediaType(DWORD* pos, IMFMediaType* pMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::AddMediaType Entry");

    HRESULT hr = E_FAIL;
    if (pMediaType != NULL)
    {
        m_listOfMediaTypes.push_back(pMediaType);
        pMediaType->AddRef();

        if (pos)
        {
            *pos = (DWORD)(m_listOfMediaTypes.size() - 1);
        }

        hr = S_OK;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CInPin::AddMediaType Exit %!HRESULT!", hr);
    return hr;
}

HRESULT CBasePin::GetMediaTypeAt(DWORD pos, IMFMediaType** pMediaType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetMediaTypeAt Entry");

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetMediaTypeAt pos: %d", pos);

    HRESULT hr = E_FAIL;

    if (pMediaType != NULL)
    {
        *pMediaType = nullptr;

        if (pos < m_listOfMediaTypes.size())
        {
            *pMediaType = m_listOfMediaTypes[pos];

            hr = S_OK;
        }
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "CBasePin::GetMediaTypeAt Exit %!HRESULT!", hr);
    return hr;
}

COutPin::COutPin(DWORD outputStreamId, IMFTransform *sourceTransform, IKsControl* iksControl) : CBasePin(outputStreamId, (MyMFT *)sourceTransform)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "COutPin::COutPin Entry");

    m_spSourceTransform = sourceTransform;
    m_spIkscontrol = iksControl;

    // Create Attribute Store
    MFCreateAttributes(&m_spAttributes, 3);

    // Set atttributes
    SetUINT32(MFT_SUPPORT_DYNAMIC_FORMAT_CHANGE, TRUE);
    SetString(MFT_ENUM_HARDWARE_URL_Attribute, L"Sample_CameraExtensionMft");
    SetUINT32(MF_TRANSFORM_ASYNC, TRUE);

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "COutPin::COutPin Exit");
}

CBasePin::CBasePin(_In_ ULONG id, _In_ MyMFT* parent) :
    m_streamId(id)
    , m_Parent(parent)
    , m_setMediaType(nullptr)
    , m_nRefCount(0)
    , m_state(DeviceStreamState_Stop)
    , m_dwWorkQueueId(MFASYNC_CALLBACK_QUEUE_UNDEFINED)
{

}

COutPin::COutPin(ULONG id, MyMFT* pparent, IKsControl* iksControl, MFSampleAllocatorUsage allocatorUsage) : CBasePin(id, pparent)
, m_firstSample(false)
, m_queue(nullptr)
, m_allocatorUsage(allocatorUsage)
{

    HRESULT                 hr = S_OK;
    ComPtr<IMFAttributes>   spAttributes;

    //
    //Get the input pin IKS control.. the pin IKS control talks to sourcetransform's IKS control
    //
    m_spIkscontrol = iksControl;

    MFCreateAttributes(&spAttributes, 3); //Create the space for the attribute store!!
    setAttributes(spAttributes.Get());
    DMFTCHECKHR_GOTO(SetUINT32(MFT_SUPPORT_DYNAMIC_FORMAT_CHANGE, TRUE), done);
    DMFTCHECKHR_GOTO(SetString(MFT_ENUM_HARDWARE_URL_Attribute, L"Sample_CameraExtensionMft"), done);
    DMFTCHECKHR_GOTO(SetUINT32(MF_TRANSFORM_ASYNC, TRUE), done);

done:
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "%!FUNC! exiting %x = %!HRESULT!", hr, hr);

}

HRESULT COutPin::GetOutputAvailableType(DWORD dwTypeIndex, IMFMediaType** ppType)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "COutPin::GetOutputAvailableType Entry");

    HRESULT hr = E_FAIL;

    if (ppType != NULL)
    {
        *ppType = nullptr;

        if (dwTypeIndex < m_listOfMediaTypes.size())
        {
            *ppType = m_listOfMediaTypes[dwTypeIndex];

            hr = S_OK;
        }
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "COutPin::GetOutputAvailableType Exit %!HRESULT!", hr);
    return hr;
}

CPinQueue::CPinQueue(DWORD _inPinId, IMFDeviceTransform* pTransform) : m_dwInPinId(_inPinId),
m_pTransform(pTransform),
m_cRef(1)
{
    m_streamCategory = GUID_NULL;
}

CPinQueue::~CPinQueue()
{
    Ctee::ReleaseTee(m_spTeer);
}

STDMETHODIMP_(VOID __stdcall) CPinQueue::InsertInternal(IMFSample* pSample)
{
    return VOID();
}

STDMETHODIMP_(HRESULT __stdcall) CPinQueue::Insert(IMFSample* pSample)
{
    UNREFERENCED_PARAMETER(pSample);
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CPinQueue::Remove(IMFSample** pSample)
{
    UNREFERENCED_PARAMETER(pSample);
    return E_NOTIMPL;
}

HRESULT __stdcall CPinQueue::RecreateTee(IMFMediaType* inMediatype, IMFMediaType* outMediatype, IUnknown* punkManager)
{
    UNREFERENCED_PARAMETER(inMediatype);
    UNREFERENCED_PARAMETER(outMediatype);
    UNREFERENCED_PARAMETER(punkManager);
    return E_NOTIMPL;
}

STDMETHODIMP_(HRESULT __stdcall) CPinQueue::RecreateTeeByAllocatorMode(IMFMediaType* inMediatype, IMFMediaType* outMediatype, IUnknown* punkManager, MFSampleAllocatorUsage allocatorUsage, IMFVideoSampleAllocator* pAllcoator)
{
    UNREFERENCED_PARAMETER(inMediatype);
    UNREFERENCED_PARAMETER(outMediatype);
    UNREFERENCED_PARAMETER(punkManager);
    UNREFERENCED_PARAMETER(allocatorUsage);
    UNREFERENCED_PARAMETER(pAllcoator);
    return E_NOTIMPL;
}

STDMETHODIMP_(VOID __stdcall) CPinQueue::Clear()
{
    return VOID();
}
