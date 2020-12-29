#pragma once
#include <mfidl.h>
#include <ks.h>
#include <ksproxy.h>
#include <Mferror.h>
#include <mfapi.h>

#include <Windows.Foundation.h>
#include <wrl\client.h>
using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;
#include <wil\result.h>
#include <wil\resource.h>
#include <wil\com.h>
#include <wil\result_macros.h>

#include <vector>

typedef std::vector<IMFMediaType*> IMFMediaTypeArray;

class CBasePin : public IMFAttributes, public IKsControl
{
private:
	ULONG m_cRef;
	ULONG m_streamId;


protected:
	ComPtr<IMFAttributes> m_spAttributes;

	IMFMediaTypeArray m_listOfMediaTypes;
	DWORD m_dwWorkQueueId;
	ComPtr<IKsControl> m_spIkscontrol;
	ComPtr<IUnknown> m_spDxgiManager;
	ComPtr<IMFMediaType> m_setMediaType;
	DeviceStreamState m_state;
	CRITICAL_SECTION m_lock;

public:
	CBasePin(DWORD streamId);

	HRESULT AddMediaType(DWORD* pos, IMFMediaType* pMediaType);
	HRESULT GetMediaTypeAt(DWORD pos, IMFMediaType** pMediaType);
	VOID SetWorkQueue(_In_ DWORD dwQueueId);
	VOID SetD3DManager(IUnknown* pManager);

	HRESULT getMediaType(IMFMediaType** ppMediaType);
	VOID setMediaType(IMFMediaType* pMediaType);
	STDMETHOD_(BOOL, IsMediaTypeSupported)(IMFMediaType* pMediaType, IMFMediaType** ppIMFMediaTypeFull);
	STDMETHODIMP_(DeviceStreamState) SetState(_In_ DeviceStreamState State);

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID, VOID**);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IKsControl
	STDMETHOD(KsEvent)(
		PKSEVENT Event,
		ULONG    EventLength,
		LPVOID   EventData,
		ULONG    DataLength,
		ULONG* BytesReturned
	);

	STDMETHOD(KsMethod)(
		PKSMETHOD Method,
		ULONG     MethodLength,
		LPVOID    MethodData,
		ULONG     DataLength,
		ULONG* BytesReturned
	);

	STDMETHOD(KsProperty)(
		PKSPROPERTY Property,
		ULONG       PropertyLength,
		LPVOID      PropertyData,
		ULONG       DataLength,
		ULONG* BytesReturned
	);

	// IMFAttributes
	STDMETHOD(Compare)(
		IMFAttributes* pTheirs,
		MF_ATTRIBUTES_MATCH_TYPE MatchType,
		BOOL* pbResult
	);
	STDMETHOD(CompareItem)(
		REFGUID        guidKey,
		REFPROPVARIANT Value,
		BOOL* pbResult
	);
	STDMETHOD(CopyAllItems)(
		IMFAttributes* pDest
	);

	STDMETHOD(DeleteAllItems)();

	STDMETHOD(DeleteItem)(
		REFGUID guidKey
	);

	STDMETHOD(GetAllocatedBlob)(
		REFGUID guidKey,
		UINT8** ppBuf,
		UINT32* pcbSize
	);

	STDMETHOD(GetAllocatedString)(
		REFGUID guidKey,
		LPWSTR* ppwszValue,
		UINT32* pcchLength
	);

	STDMETHOD(GetBlob)(
		REFGUID guidKey,
		UINT8* pBuf,
		UINT32  cbBufSize,
		UINT32* pcbBlobSize
	);

	STDMETHOD(GetBlobSize)(
		REFGUID guidKey,
		UINT32* pcbBlobSize
	);

	STDMETHOD(GetCount)(
		UINT32* pcItems
	);

	STDMETHOD(GetDouble)(
		REFGUID guidKey,
		double* pfValue
	);

	STDMETHOD(GetGUID)(
		REFGUID guidKey,
		GUID* pguidValue
	);

	STDMETHOD(GetItem)(
		REFGUID     guidKey,
		PROPVARIANT* pValue
	);

	STDMETHOD(GetItemByIndex)(
		UINT32      unIndex,
		GUID* pguidKey,
		PROPVARIANT* pValue
	);

	STDMETHOD(GetItemType)(
		REFGUID           guidKey,
		MF_ATTRIBUTE_TYPE* pType
	);

	STDMETHOD(GetString)(
		REFGUID guidKey,
		LPWSTR  pwszValue,
		UINT32  cchBufSize,
		UINT32* pcchLength
	);

	STDMETHOD(GetStringLength)(
		REFGUID guidKey,
		UINT32* pcchLength
	);

	STDMETHOD(GetUINT32)(
		REFGUID guidKey,
		UINT32* punValue
	);

	STDMETHOD(GetUINT64)(
		REFGUID guidKey,
		UINT64* punValue
	);

	STDMETHOD(GetUnknown)(
		REFGUID guidKey,
		REFIID  riid,
		LPVOID* ppv
	);

	STDMETHOD(LockStore)();

	STDMETHOD(SetBlob)(
		REFGUID     guidKey,
		const UINT8* pBuf,
		UINT32      cbBufSize
	);

	STDMETHOD(SetDouble)(
		REFGUID guidKey,
		double  fValue
	);

	STDMETHOD(SetGUID)(
		REFGUID guidKey,
		REFGUID guidValue
	);

	STDMETHOD(SetItem)(
		REFGUID        guidKey,
		REFPROPVARIANT Value
	);

	STDMETHOD(SetString)(
		REFGUID guidKey,
		LPCWSTR wszValue
	);

	STDMETHOD(SetUINT32)(
		REFGUID guidKey,
		UINT32  unValue
	);

	STDMETHOD(SetUINT64)(
		REFGUID guidKey,
		UINT64  unValue
	);

	STDMETHOD(SetUnknown)(
		REFGUID  guidKey,
		IUnknown* pUnknown
	);

	STDMETHOD(UnlockStore)();

	DWORD GetStreamId();
};

class CInPin : public CBasePin
{
public:
	CInPin(IMFAttributes* pAttributes, DWORD inputStreamId);
	~CInPin();
	STDMETHODIMP Init(IMFTransform* pTransform);
	VOID setPreferredMediaType(IMFMediaType* pMediaType);
	DeviceStreamState setPreferredStreamState(DeviceStreamState streamState);
	STDMETHODIMP WaitForSetInputPinMediaChange();
	HRESULT GetInputStreamPreferredState(DeviceStreamState* value, IMFMediaType** ppMediaType);
	HRESULT SetInputStreamState(IMFMediaType* pMediaType, DeviceStreamState value, DWORD dwFlags);

private:
	ComPtr<IMFTransform> m_spSourceTransform;
	GUID m_stStreamType;
	HANDLE m_waitInputMediaTypeWaiter;
	ComPtr<IMFMediaType> m_spPrefferedMediaType;
	DeviceStreamState m_preferredStreamState;

	HRESULT GenerateMFMediaTypeListFromDevice(UINT uiStreamId);

};

class CAsyncInPin : public CInPin
{

};

class COutPin : public CBasePin
{
public:
	COutPin(DWORD outputStreamId, IMFTransform *sourceTransform, IKsControl* iksControl);
	
	HRESULT GetOutputAvailableType(DWORD dwTypeIndex, IMFMediaType** ppType);
	STDMETHODIMP ChangeMediaTypeFromInpin(IMFMediaType* pInMediatype, IMFMediaType* pOutMediaType, DeviceStreamState state);
	STDMETHODIMP_(VOID) SetFirstSample(BOOL);

private:
	GUID m_stStreamType;
	ComPtr<IMFTransform> m_spSourceTransform;
	BOOL m_firstSample;
};

typedef std::vector<CBasePin*> CBasePinArray;


