#pragma once
#include "pch.h"

#include <mfidl.h>
#include <ks.h>
#include <ksproxy.h>
#include <mfapi.h>

#include <Windows.Foundation.h>
#include <wrl\client.h>
using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;
#include <wil\result.h>
#include <wil\resource.h>
#include <wil\com.h>
#include <wil\result_macros.h>
#include "common.h"

#include <map>
using namespace std;

#include <initguid.h>
DEFINE_GUID(AVSTREAM_CUSTOM_PIN_IMAGE,
	0x888c4105, 0xb328, 0x4ed6, 0xa3, 0xca, 0x2f, 0xf4, 0xc0, 0x3a, 0x9f, 0x33);


class MyMFT : public IMFRealTimeClientEx, public IKsControl, public IMFSampleAllocatorControl, public IMFDeviceTransform, public IMFShutdown, public IMFMediaEventGenerator,
	public IMFTransform
{
	friend class CPinCreationFactory;

public:
	MyMFT();
	~MyMFT();

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
	STDMETHODIMP GetStreamCount(DWORD*,	DWORD*);
	STDMETHODIMP GetStreamIDs(DWORD, DWORD*, DWORD,	DWORD*);
	STDMETHODIMP InitializeTransform(_In_ IMFAttributes*);
	STDMETHODIMP ProcessEvent(DWORD, IMFMediaEvent*);
	STDMETHODIMP ProcessInput(DWORD, IMFSample*, DWORD);
	STDMETHODIMP ProcessMessage(MFT_MESSAGE_TYPE, ULONG_PTR);
	STDMETHODIMP ProcessOutput(DWORD, DWORD, MFT_OUTPUT_DATA_BUFFER*, DWORD*);
	STDMETHODIMP SetInputStreamState(DWORD, IMFMediaType*, DeviceStreamState, DWORD);
	STDMETHODIMP SetOutputStreamState(DWORD, IMFMediaType*, DeviceStreamState, DWORD);
	STDMETHODIMP_(VOID) FlushAllStreams(
		VOID
	);

	// IMFShutdown
	STDMETHODIMP GetShutdownStatus(MFSHUTDOWN_STATUS*);
	STDMETHODIMP Shutdown();

	// IMFMediaEventGenerator
	// https://docs.microsoft.com/en-us/windows/win32/medfound/media-event-generators
	STDMETHODIMP BeginGetEvent(IMFAsyncCallback*, IUnknown*);
	STDMETHODIMP EndGetEvent(IMFAsyncResult*, IMFMediaEvent**);
	STDMETHODIMP GetEvent(DWORD, IMFMediaEvent**);
	STDMETHODIMP QueueEvent(MediaEventType, REFGUID, HRESULT, const PROPVARIANT*);

	// IMFTransform
	STDMETHODIMP AddInputStreams(
		DWORD cStreams,
		DWORD* adwStreamIDs
	);

	STDMETHODIMP DeleteInputStream(
		DWORD dwStreamID
	);

	STDMETHODIMP GetAttributes(
		IMFAttributes** pAttributes
	);

	//STDMETHODIMP GetInputAvailableType(DWORD        dwInputStreamID, DWORD        dwTypeIndex, IMFMediaType** ppType);
	//STDMETHODIMP GetInputCurrentType(DWORD        dwInputStreamID, IMFMediaType** ppType);
	STDMETHODIMP GetInputStatus(
		DWORD dwInputStreamID,
		DWORD* pdwFlags
	);

	//HRESULT GetInputStreamAttributes(DWORD         dwInputStreamID, IMFAttributes** pAttributes);

	STDMETHODIMP GetInputStreamInfo(
		DWORD                 dwInputStreamID,
		MFT_INPUT_STREAM_INFO* pStreamInfo
	);

	//HRESULT GetOutputAvailableType(DWORD        dwOutputStreamID, DWORD        dwTypeIndex, IMFMediaType** ppType);
	//HRESULT GetOutputCurrentType(DWORD        dwOutputStreamID, IMFMediaType** ppType);

	STDMETHODIMP GetOutputStatus(
		DWORD* pdwFlags
	);

	//HRESULT GetOutputStreamAttributes(DWORD         dwOutputStreamID, IMFAttributes** pAttributes);
	STDMETHODIMP GetOutputStreamInfo(
		DWORD                  dwOutputStreamID,
		MFT_OUTPUT_STREAM_INFO* pStreamInfo
	);
	//HRESULT GetStreamCount(DWORD* pcInputStreams, DWORD* pcOutputStreams);
	//HRESULT GetStreamIDs(DWORD dwInputIDArraySize, DWORD* pdwInputIDs, DWORD dwOutputIDArraySize, DWORD* pdwOutputIDs);
	STDMETHODIMP GetStreamLimits(
		DWORD* pdwInputMinimum,
		DWORD* pdwInputMaximum,
		DWORD* pdwOutputMinimum,
		DWORD* pdwOutputMaximum
	);
	//HRESULT ProcessEvent(DWORD         dwInputStreamID, IMFMediaEvent* pEvent);
	//HRESULT ProcessInput(DWORD     dwInputStreamID, IMFSample* pSample, DWORD     dwFlags);
	//HRESULT ProcessMessage(MFT_MESSAGE_TYPE eMessage, ULONG_PTR        ulParam);
	// HRESULT ProcessOutput(DWORD                  dwFlags, DWORD                  cOutputBufferCount, MFT_OUTPUT_DATA_BUFFER* pOutputSamples, DWORD* pdwStatus);
	STDMETHODIMP SetInputType(
		DWORD        dwInputStreamID,
		IMFMediaType* pType,
		DWORD        dwFlags
	);
	STDMETHODIMP SetOutputBounds(
		LONGLONG hnsLowerBound,
		LONGLONG hnsUpperBound
	);
	STDMETHODIMP SetOutputType(
		DWORD        dwOutputStreamID,
		IMFMediaType* pType,
		DWORD        dwFlags
	);

	_DEFINE_DEVICEMFT_MFT0HELPER_IMPL__
		
	_inline IMFTransform* Parent()
	{
		return m_spSourceTransform.Get();
	}
private:
	ULONG m_cRef;


	ULONG m_InputPinCount;
	ULONG m_OutputPinCount;

	//CBasePinArray m_basePinArrayInPins;
	//CBasePinArray m_basePinArrayOutPins;

	CInPin* GetInPin(DWORD dwStreamId);
	COutPin* GetOutPin(DWORD dwStreamId);

	// https://docs.microsoft.com/en-us/windows/win32/medfound/media-event-generators
	CRITICAL_SECTION m_critSec;
	IMFMediaEventQueue* m_pQueue;

	ComPtr<IMFTransform> m_spSourceTransform;

	HRESULT BridgeInputPinOutputPin(CInPin* pInPin, COutPin* pOutPin);
	STDMETHODIMP CheckCustomPin(
		_In_ CInPin* pPin,
		_Inout_ PBOOL  pIsCustom
	);

	PWCHAR m_SymbolicLink;
	ComPtr<IKsControl> m_spIkscontrol;
	ULONG m_CustomPinCount;
	CBasePinArray m_InPins;
	CBasePinArray m_OutPins;
	map<int, int> m_outputPinMap;
};

typedef MyMFT* PMyMFT;

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
	HRESULT CreatePin(_In_ ULONG ulInputStreamId, _In_ ULONG ulOutStreamId, _In_ type_pin type, _Outptr_ CBasePin** ppPin, _In_ BOOL& isCustom);
	CPinCreationFactory(_In_ MyMFT* pDeviceTransform) :m_spDeviceTransform(pDeviceTransform) {
	}
};
