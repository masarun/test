#pragma once

#include <mftransform.h>
#include <mfidl.h>
#include <ks.h>
#include <ksproxy.h>
#include <ksmedia.h>

#include <Windows.Foundation.h>
#include <wrl\client.h>
using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;

#include <comdef.h>

#include <map>
#include <stdexcept>
using namespace std;

#include "Pin.h"
#include "common.h"

#if !defined DMFTCHECKHR_GOTO
#define DMFTCHECKHR_GOTO(a,b) {hr=(a); if(FAILED(hr)){goto b;}} 
#endif

#if !defined DMFTCHECKNULL_GOTO
#define DMFTCHECKNULL_GOTO(a,b,c) {if(!a) {hr = c; goto b;}} 
#endif

class CBasePin;
class CInPin;
class COutPin;

class MyMFT : 
	public IMFDeviceTransform,
	public IMFTransform,
	public IMFShutdown,
	public IMFMediaEventGenerator,
	public IMFRealTimeClientEx,
	public IKsControl,
	public IMFSampleAllocatorControl
{
	friend class CPinCreationFactory;

public:
	MyMFT();

	// IUknown
	STDMETHODIMP QueryInterface(REFIID, VOID**);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IMFRealTimeClientEx
	STDMETHODIMP RegisterThreadsEx(DWORD*, LPCWSTR, LONG);
	STDMETHODIMP SetWorkQueueEx(DWORD, LONG);
	STDMETHODIMP UnregisterThreads();

	// IKsControl
	STDMETHODIMP KsEvent(PKSEVENT, ULONG, LPVOID, ULONG, ULONG*);
	STDMETHODIMP KsMethod(PKSMETHOD, ULONG, LPVOID, ULONG, ULONG*);
	STDMETHODIMP KsProperty(PKSPROPERTY, ULONG, LPVOID, ULONG, ULONG*);

	// IMFSampleAllocatorControl
	STDMETHODIMP GetAllocatorUsage(DWORD, DWORD*, MFSampleAllocatorUsage*);
	STDMETHODIMP SetDefaultAllocator(DWORD, IUnknown*);

	// IMFDeviceTransform
	STDMETHODIMP FlushInputStream(DWORD, DWORD);
	STDMETHODIMP FlushOutputStream(DWORD, DWORD);
	STDMETHODIMP GetInputAvailableType(DWORD, DWORD, IMFMediaType**);
	STDMETHODIMP GetInputCurrentType(DWORD, IMFMediaType**);
	STDMETHODIMP GetInputStreamAttributes(DWORD, IMFAttributes**);
	STDMETHODIMP GetInputStreamPreferredState(DWORD, DeviceStreamState*, IMFMediaType**);
	STDMETHODIMP GetInputStreamState(DWORD, DeviceStreamState*);
	STDMETHODIMP GetOutputAvailableType(DWORD, DWORD, IMFMediaType**);
	STDMETHODIMP GetOutputCurrentType(DWORD, IMFMediaType**);
	STDMETHODIMP GetOutputStreamAttributes(DWORD, IMFAttributes**);
	STDMETHODIMP GetOutputStreamState(DWORD, DeviceStreamState*);
	STDMETHODIMP GetStreamCount(DWORD*, DWORD*);
	STDMETHODIMP GetStreamIDs(DWORD, DWORD*, DWORD, DWORD*);
	STDMETHODIMP InitializeTransform(_In_ IMFAttributes*);
	STDMETHODIMP ProcessEvent(DWORD, IMFMediaEvent*);
	STDMETHODIMP ProcessInput(DWORD, IMFSample*, DWORD);
	STDMETHODIMP ProcessMessage(MFT_MESSAGE_TYPE, ULONG_PTR);
	STDMETHODIMP ProcessOutput(DWORD, DWORD, MFT_OUTPUT_DATA_BUFFER*, DWORD*);
	STDMETHODIMP SetInputStreamState(DWORD, IMFMediaType*, DeviceStreamState, DWORD);
	STDMETHODIMP SetOutputStreamState(DWORD, IMFMediaType*, DeviceStreamState, DWORD);
	STDMETHODIMP_(VOID) FlushAllStreams(VOID);

	// IMFShutdown
	STDMETHODIMP GetShutdownStatus(MFSHUTDOWN_STATUS*);
	STDMETHODIMP Shutdown();

	// IMFMediaEventGenerator
	// https://docs.microsoft.com/en-us/windows/win32/medfound/media-event-generators
	STDMETHODIMP BeginGetEvent(IMFAsyncCallback*, IUnknown*);
	STDMETHODIMP EndGetEvent(IMFAsyncResult*, IMFMediaEvent**);
	STDMETHODIMP GetEvent(DWORD, IMFMediaEvent**);
	STDMETHODIMP QueueEvent(MediaEventType, REFGUID, HRESULT, const PROPVARIANT*);

	STDMETHOD(QueueEvent)(IMFMediaEvent*);

	// IMFTransform
	STDMETHODIMP AddInputStreams(DWORD, DWORD*);
	STDMETHODIMP DeleteInputStream(DWORD);
	STDMETHODIMP GetAttributes(IMFAttributes**);
	STDMETHODIMP GetInputStatus(DWORD, DWORD*);
	STDMETHODIMP GetInputStreamInfo(DWORD, MFT_INPUT_STREAM_INFO*);
	STDMETHODIMP GetOutputStatus(DWORD*);
	STDMETHODIMP GetOutputStreamInfo(DWORD, MFT_OUTPUT_STREAM_INFO*);
	STDMETHODIMP GetStreamLimits(DWORD*, DWORD*, DWORD*, DWORD*);
	STDMETHODIMP SetInputType(DWORD, IMFMediaType*, DWORD);
	STDMETHODIMP SetOutputBounds(LONGLONG, LONGLONG);
	STDMETHODIMP SetOutputType(DWORD, IMFMediaType*, DWORD);
	// Duplicated with IMFTransform
	//STDMETHODIMP GetInputAvailableType(DWORD dwInputStreamID, DWORD dwTypeIndex, IMFMediaType** ppType);
	//STDMETHODIMP GetInputCurrentType(DWORD dwInputStreamID, IMFMediaType** ppType);
	//HRESULT GetInputStreamAttributes(DWORD dwInputStreamID, IMFAttributes** pAttributes);
	//HRESULT GetOutputAvailableType(DWORD dwOutputStreamID, DWORD dwTypeIndex, IMFMediaType** ppType);
	//HRESULT GetOutputCurrentType(DWORD dwOutputStreamID, IMFMediaType** ppType);
	//HRESULT GetOutputStreamAttributes(DWORD dwOutputStreamID, IMFAttributes** pAttributes);
	//HRESULT GetStreamCount(DWORD* pcInputStreams, DWORD* pcOutputStreams);
	//HRESULT GetStreamIDs(DWORD dwInputIDArraySize, DWORD* pdwInputIDs, DWORD dwOutputIDArraySize, DWORD* pdwOutputIDs);
	//HRESULT ProcessEvent(DWORD dwInputStreamID, IMFMediaEvent* pEvent);
	//HRESULT ProcessInput(DWORD dwInputStreamID, IMFSample* pSample, DWORD dwFlags);
	//HRESULT ProcessMessage(MFT_MESSAGE_TYPE eMessage, ULONG_PTR ulParam);
	//HRESULT ProcessOutput(DWORD dwFlags, DWORD cOutputBufferCount, MFT_OUTPUT_DATA_BUFFER* pOutputSamples, DWORD* pdwStatus);

	HRESULT SendEventToManager(MediaEventType, REFGUID, UINT32);

protected:

	HRESULT BridgeInputPinOutputPin(CInPin*, COutPin*);
	CInPin* GetInPin(DWORD);
	COutPin* GetOutPin(DWORD);
	HRESULT GetConnectedInpin(ULONG, ULONG&);

	//
	//Inline functions
	//

	__inline IMFTransform* Parent()
	{
		return m_spSourceTransform.Get();
	}

	__requires_lock_held(m_critSec)
	HRESULT ChangeMediaTypeEx(ULONG, IMFMediaType*, DeviceStreamState);

	/*
	STDMETHOD(MFTGetStreamCount)(DWORD* pdwInputStreams, DWORD* pdwOutputStreams)
	{
		return GetStreamCount(pdwInputStreams, pdwOutputStreams);
	}

	STDMETHOD(MFTGetStreamIDs)(DWORD  dwInputIDArraySize, DWORD* pdwInputIDs, DWORD  dwOutputIDArraySize, DWORD* pdwOutputIDs)
	{
		return GetStreamIDs(dwInputIDArraySize,
			pdwInputIDs,
			dwOutputIDArraySize,
			pdwOutputIDs);
	}

	STDMETHOD(MFTGetOutputAvailableType)(DWORD dwOutputStreamID, DWORD dwTypeIndex, IMFMediaType** ppMediaType)                                             
	{                                                 
		return GetOutputAvailableType(
			dwOutputStreamID, dwTypeIndex, ppMediaType);
	}
	*/
private:
	ULONG m_cRef;
	ComPtr<IMFTransform> m_spSourceTransform;
	PWCHAR m_SymbolicLink;
	ComPtr<IKsControl> m_spIkscontrol;
	std::vector<CBasePin*> m_OutPins;
	std::vector<CBasePin*> m_InPins;
	map<int, int> m_outputPinMap;
	ULONG m_InputPinCount;
	ULONG m_OutputPinCount;
	CCritSec m_critSec;
	CCritSec m_critSecForEvent;
	IMFMediaEventQueue* m_pQueue;
	DWORD m_dwWorkQueueId;
	LONG m_lWorkQueuePriority;
};

class CPinCreationFactory {
protected:
	ComPtr<MyMFT> m_spDeviceTransform;
public:
	typedef enum _type_pin {
		DMFT_PIN_INPUT,
		DMFT_PIN_OUTPUT,
		DMFT_PIN_CUSTOM,
		DMFT_PIN_ALLOCATOR_PIN,
		DMFT_MAX
	}type_pin;
	HRESULT CreatePin(ULONG ulInputStreamId, ULONG ulOutStreamId, type_pin type, CBasePin** ppPin, BOOL& isCustom);
	CPinCreationFactory(MyMFT* pDeviceTransform) :m_spDeviceTransform(pDeviceTransform) {
	}
};

#define MEDIAPRINTER_STARTLEN  (512)
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
#ifndef IF_EQUAL_RETURN
#define IF_EQUAL_RETURN(param, val) if(val == param) return #val
#endif
class CMediaTypePrinter {
private:
	IMFMediaType* pMediaType;
	PCHAR                m_pBuffer;
	ULONG                buffLen;
public:
	CMediaTypePrinter(_In_ IMFMediaType* _pMediaType);
	~CMediaTypePrinter();
	STDMETHODIMP_(PCHAR) ToCompleteString();
	STDMETHODIMP_(PCHAR) ToString();
};

template <typename Lambda>
HRESULT ExceptionBoundary(Lambda&& lambda)
{
	try
	{
		lambda();
		return S_OK;
	}
	catch (const _com_error& e)
	{
		return e.Error();
	}
	catch (const std::bad_alloc&)
	{
		return E_OUTOFMEMORY;
	}
	catch (const std::out_of_range&)
	{
		return MF_E_INVALIDINDEX;
	}
	catch (...)
	{
		return E_UNEXPECTED;
	}
}

#define SAFE_DELETE(p)              delete p; p = NULL;
#define SAFERELEASE(x) \
if (x) {\
    x->Release(); \
    x = NULL; \
}

#define IsEqualCLSID(rclsid1, rclsid2) IsEqualGUID(rclsid1, rclsid2)

#ifdef DBG
#define mf_assert(a) if(!a) DebugBreak()
#else
#define mf_assert(a)
#endif
