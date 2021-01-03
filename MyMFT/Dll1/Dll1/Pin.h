#pragma once
#include <mfidl.h>
#include <ks.h>
#include <ksproxy.h>
#include <Mferror.h>
#include <mfapi.h>

#include <vector>

#include <wil\com.h>

#include "MyMFT.h"
#include "common.h"

class MyMFT;

class CBasePin :
	public IMFAttributes,
	public IKsControl
{
public:
	CBasePin(ULONG, MyMFT*);
	~CBasePin();

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID, VOID**);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IKsControl
	STDMETHOD(KsEvent)(PKSEVENT, ULONG, LPVOID, ULONG, ULONG*);
	STDMETHOD(KsMethod)(PKSMETHOD, ULONG, LPVOID, ULONG, ULONG*);
	STDMETHOD(KsProperty)(PKSPROPERTY, ULONG, LPVOID, ULONG, ULONG*);

	// IMFAttributes
	STDMETHOD(Compare)(IMFAttributes*, MF_ATTRIBUTES_MATCH_TYPE, BOOL*);
	STDMETHOD(CompareItem)(REFGUID, REFPROPVARIANT, BOOL*);
	STDMETHOD(CopyAllItems)(IMFAttributes*);
	STDMETHOD(DeleteAllItems)();
	STDMETHOD(DeleteItem)(REFGUID);
	STDMETHOD(GetAllocatedBlob)(REFGUID, UINT8**, UINT32*);
	STDMETHOD(GetAllocatedString)(REFGUID, LPWSTR*, UINT32*);
	STDMETHOD(GetBlob)(REFGUID, UINT8*, UINT32, UINT32*);
	STDMETHOD(GetBlobSize)(REFGUID, UINT32*);
	STDMETHOD(GetCount)(UINT32*);
	STDMETHOD(GetDouble)(REFGUID, double*);
	STDMETHOD(GetGUID)(REFGUID, GUID*);
	STDMETHOD(GetItem)(REFGUID, PROPVARIANT*);
	STDMETHOD(GetItemByIndex)(UINT32, GUID*, PROPVARIANT*);
	STDMETHOD(GetItemType)(REFGUID, MF_ATTRIBUTE_TYPE*);
	STDMETHOD(GetString)(REFGUID, LPWSTR, UINT32, UINT32*);
	STDMETHOD(GetStringLength)(REFGUID, UINT32*);
	STDMETHOD(GetUINT32)(REFGUID, UINT32*);
	STDMETHOD(GetUINT64)(REFGUID, UINT64*);
	STDMETHOD(GetUnknown)(REFGUID, REFIID, LPVOID*);
	STDMETHOD(LockStore)();
	STDMETHOD(SetBlob)(REFGUID, const UINT8*, UINT32);
	STDMETHOD(SetDouble)(REFGUID, double);
	STDMETHOD(SetGUID)(REFGUID, REFGUID);
	STDMETHOD(SetItem)(REFGUID ,REFPROPVARIANT);
	STDMETHOD(SetString)(REFGUID, LPCWSTR);
	STDMETHOD(SetUINT32)(REFGUID, UINT32);
	STDMETHOD(SetUINT64)(REFGUID, UINT64);
	STDMETHOD(SetUnknown)(REFGUID, IUnknown*);
	STDMETHOD(UnlockStore)();

	virtual STDMETHODIMP_(DeviceStreamState) SetState(_In_ DeviceStreamState State);

	__inline DWORD streamId()
	{
		return m_StreamId;
	}

	__inline  STDMETHOD(getPinAttributes) (IMFAttributes** ppAttributes)
	{
		return QueryInterface(IID_PPV_ARGS(ppAttributes));
	}

	__inline VOID setMediaType(IMFMediaType* pMediaType)
	{
		m_setMediaType = pMediaType;
	}

	__inline HRESULT getMediaType(IMFMediaType** ppMediaType)
	{
		HRESULT hr = S_OK;
		if (!ppMediaType)
			return E_INVALIDARG;

		if (m_setMediaType != nullptr)
		{
			hr = m_setMediaType.CopyTo(ppMediaType);
		}
		else
		{
			hr = MF_E_TRANSFORM_TYPE_NOT_SET;
		}
		return hr;
	}

	STDMETHOD(AddMediaType)(DWORD*, IMFMediaType*);
	STDMETHODIMP GetMediaTypeAt(DWORD, IMFMediaType**);
	STDMETHODIMP GetOutputAvailableType(DWORD, IMFMediaType**);
	STDMETHOD_(BOOL, IsMediaTypeSupported)(IMFMediaType*, IMFMediaType**);

	VOID SetWorkQueue(DWORD dwQueueId)
	{
		m_dwWorkQueueId = dwQueueId;
	}

protected:
	DeviceStreamState m_state;
	DWORD m_dwWorkQueueId;
	ComPtr<IMFAttributes> m_spAttributes;
	std::vector<IMFMediaType*> m_listOfMediaTypes;
	ComPtr<IKsControl> m_spIkscontrol;

	__inline HRESULT setAttributes(_In_ IMFAttributes* _pAttributes)
	{
		m_spAttributes = _pAttributes;
		return S_OK;
	}

	__inline CCritSec& lock()
	{
		return m_lock;
	}

private:
	ULONG m_cRef;
	ULONG m_StreamId;
	MyMFT* m_Parent;
	ComPtr<IMFMediaType> m_setMediaType;
	CCritSec m_lock;
};

class CInPin : public CBasePin
{
public:
	CInPin(IMFAttributes*, ULONG, MyMFT*);
	~CInPin();

	STDMETHOD(Init)(IMFTransform*);
	HRESULT GenerateMFMediaTypeListFromDevice(UINT);
	STDMETHOD_(VOID, ConnectPin)(CBasePin*);
	STDMETHODIMP WaitForSetInputPinMediaChange();

	HRESULT GetInputStreamPreferredState(DeviceStreamState*, IMFMediaType**);
	HRESULT SetInputStreamState(IMFMediaType*, DeviceStreamState, DWORD);

	__inline VOID setPreferredMediaType(IMFMediaType* pMediaType)
	{
		m_spPrefferedMediaType = pMediaType;
	}
	__inline DeviceStreamState setPreferredStreamState(DeviceStreamState streamState)
	{
		return (DeviceStreamState)InterlockedCompareExchange((LONG*)&m_preferredStreamState, (LONG)streamState, (LONG)m_preferredStreamState);
	}

private:
	ComPtr<IMFTransform> m_spSourceTransform;
	HANDLE m_waitInputMediaTypeWaiter;
	DeviceStreamState m_preferredStreamState;
	GUID m_stStreamType;
	ComPtr<IMFMediaType> m_spPrefferedMediaType;
};

class COutPin : public CBasePin
{
public:
	COutPin(ULONG id = 0, MyMFT* pparent = NULL, IKsControl* iksControl = NULL, MFSampleAllocatorUsage allocatorUsage = MFSampleAllocatorUsage_DoesNotAllocate);

	STDMETHODIMP AddPin(DWORD);
	STDMETHODIMP_(VOID) SetFirstSample(BOOL);
	STDMETHODIMP ChangeMediaTypeFromInpin(IMFMediaType*, IMFMediaType*, DeviceStreamState);

private:
	BOOL                      m_firstSample;
	MFSampleAllocatorUsage    m_allocatorUsage;
	wil::com_ptr_nothrow<IMFVideoSampleAllocator> m_spDefaultAllocator;
};

#if !defined DMFTCHECKHR_GOTO
#define DMFTCHECKHR_GOTO(a,b) {hr=(a); if(FAILED(hr)){goto b;}} 
#endif

#if !defined DMFTCHECKNULL_GOTO
#define DMFTCHECKNULL_GOTO(a,b,c) {if(!a) {hr = c; goto b;}} 
#endif
