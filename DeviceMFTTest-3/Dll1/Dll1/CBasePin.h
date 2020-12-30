#pragma once
#include "pch.h"

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

class MyMFT;
class CPinQueue;

class CCritSec
{
private:
	CRITICAL_SECTION m_criticalSection;
public:
	CCritSec();
	~CCritSec();
	_Requires_lock_not_held_(m_criticalSection) _Acquires_lock_(m_criticalSection)
		void Lock();
	_Requires_lock_held_(m_criticalSection) _Releases_lock_(m_criticalSection)
		void Unlock();
};

class CAutoLock
{
protected:
	CCritSec* m_pCriticalSection;
public:
	_Acquires_lock_(this->m_pCriticalSection->m_criticalSection)
		CAutoLock(CCritSec& crit);
	_Acquires_lock_(this->m_pCriticalSection->m_criticalSection)
		CAutoLock(CCritSec* crit);
	_Releases_lock_(this->m_pCriticalSection->m_criticalSection)
		~CAutoLock();
};

class CBasePin : public IMFAttributes, public IKsControl
{
private:
	ULONG m_cRef;
	ULONG m_streamId;
	ComPtr<IMFMediaType> m_setMediaType;
	MyMFT* m_Parent;
	ULONG  m_nRefCount;
	CCritSec m_lock;

protected:
	ComPtr<IMFAttributes> m_spAttributes;

	IMFMediaTypeArray m_listOfMediaTypes;

	DeviceStreamState       m_state;
	ComPtr<IUnknown>        m_spDxgiManager;
	DWORD                   m_dwWorkQueueId;
	
	__inline HRESULT setAttributes(_In_ IMFAttributes* _pAttributes)
	{
		m_spAttributes = _pAttributes;
		return S_OK;
	}
	__inline CCritSec& lock()
	{
		return m_lock;
	}
	_inline MyMFT* Parent()
	{
		return m_Parent;
	}
public:
	CBasePin(DWORD streamId);
	CBasePin(_In_ ULONG _id = 0, _In_ MyMFT* parent = NULL);

	HRESULT AddMediaType(DWORD* pos, IMFMediaType* pMediaType);
	HRESULT GetMediaTypeAt(DWORD pos, IMFMediaType** pMediaType);

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

	__inline DWORD streamId()
	{
		return m_streamId;
	}
};

class CInPin : public CBasePin
{
public:
	CInPin(IMFAttributes* pAttributes, DWORD inputStreamId, MyMFT* parent);
	~CInPin();
	STDMETHODIMP Init(IMFTransform* pTransform);
	STDMETHOD_(VOID, ConnectPin)(
		_In_ CBasePin*
		);
private:
	ComPtr<IMFTransform> m_spSourceTransform;
	GUID m_stStreamType;
	ComPtr<IKsControl> m_spIkscontrol;
	HANDLE m_waitInputMediaTypeWaiter;
	ComPtr<CBasePin> m_outpin;

	HRESULT GenerateMFMediaTypeListFromDevice(UINT uiStreamId);

};

class CAsyncInPin : public CInPin
{

};

class COutPin : public CBasePin
{
public:
	COutPin(DWORD outputStreamId, IMFTransform *sourceTransform, IKsControl* iksControl);
	COutPin(
		_In_ ULONG         id = 0,
		_In_opt_  MyMFT* pparent = NULL,
		_In_     IKsControl* iksControl = NULL, 
		_In_     MFSampleAllocatorUsage allocatorUsage = MFSampleAllocatorUsage_DoesNotAllocate
	);

	HRESULT GetOutputAvailableType(DWORD dwTypeIndex, IMFMediaType** ppType);
	STDMETHODIMP AddPin(
		_In_ DWORD pinId
	);
private:
	GUID m_stStreamType;
	ComPtr<IKsControl> m_spIkscontrol;
	ComPtr<IMFTransform> m_spSourceTransform;

protected:
	CPinQueue* m_queue;           /* Queue where the sample will be stored*/
	BOOL                      m_firstSample;
	MFSampleAllocatorUsage    m_allocatorUsage;
	wil::com_ptr_nothrow<IMFVideoSampleAllocator> m_spDefaultAllocator;
};

typedef std::vector<CBasePin*> CBasePinArray;
typedef  std::vector< IMFSample*>    IMFSampleList;

class Ctee : public IUnknown {
public:
	// This is a helper class to release the interface
	// It will first call shutdowntee to break any circular
	// references any components might have with their composed
	// objects
	static VOID ReleaseTee(_In_ ComPtr<Ctee>& tee)
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
	virtual STDMETHODIMP PassThrough(_In_ IMFSample*) = 0;

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

class CPinQueue : public IUnknown {
public:
	CPinQueue(_In_ DWORD _inPinId, _In_ IMFDeviceTransform* pTransform = nullptr);
	~CPinQueue();

	STDMETHODIMP_(VOID) InsertInternal(_In_  IMFSample* pSample = nullptr);
	STDMETHODIMP Insert(_In_ IMFSample* pSample);
	STDMETHODIMP Remove(_Outptr_result_maybenull_ IMFSample** pSample);
	virtual STDMETHODIMP RecreateTee(
		_In_  IMFMediaType* inMediatype,
		_In_ IMFMediaType* outMediatype,
		_In_opt_ IUnknown* punkManager);
#if ((defined NTDDI_WIN10_VB) && (NTDDI_VERSION >= NTDDI_WIN10_VB))
	STDMETHODIMP RecreateTeeByAllocatorMode(
		_In_  IMFMediaType* inMediatype,
		_In_ IMFMediaType* outMediatype,
		_In_opt_ IUnknown* punkManager,
		_In_ MFSampleAllocatorUsage allocatorUsage,
		_In_opt_ IMFVideoSampleAllocator* pAllcoator);
#endif
	STDMETHODIMP_(VOID) Clear();

	//
	//Inline functions
	//
	__inline BOOL Empty()
	{
		return (!m_sampleList.size());
	}
	__inline DWORD pinStreamId()
	{
		return m_dwInPinId;
	}
	__inline GUID pinCategory()
	{
		if (IsEqualCLSID(m_streamCategory, GUID_NULL))
		{
			ComPtr<IMFAttributes> spAttributes;
			if (SUCCEEDED(m_pTransform->GetOutputStreamAttributes(pinStreamId(), spAttributes.ReleaseAndGetAddressOf())))
			{
				(VOID)spAttributes->GetGUID(MF_DEVICESTREAM_STREAM_CATEGORY, &m_streamCategory);

			}
		}
		return m_streamCategory;
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

private:
	DWORD                m_dwInPinId;           /* This is the input pin       */
	IMFSampleList        m_sampleList;          /* List storing the samples    */
	IMFDeviceTransform* m_pTransform;         /* Weak reference to the the device MFT */
	GUID                 m_streamCategory;
	ULONG                m_cRef;
protected:
	ComPtr<Ctee>         m_spTeer;                /*Tee that acts as a passthrough or an XVP  */
};

