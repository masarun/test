#pragma once

#include <mfidl.h>
#include <ks.h>
#include <ksproxy.h>
#include <mfapi.h>
#include <d3d9.h>
#include <dxva2api.h>

#include <Windows.Foundation.h>
#include <wrl\client.h>
using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;
#include <wil\result.h>
#include <wil\resource.h>
#include <wil\com.h>
#include <wil\result_macros.h>

#include "pch.h"

class MyMFT : public IMFRealTimeClientEx, public IKsControl, public IMFSampleAllocatorControl, public IMFDeviceTransform, public IMFShutdown, public IMFMediaEventGenerator,
	public IMFTransform
{
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

private:
	ULONG m_cRef;


	ULONG m_InputPinCount;
	ULONG m_OutputPinCount;

	CBasePinArray m_basePinArrayInPins;
	CBasePinArray m_basePinArrayOutPins;

	CInPin* GetInPin(DWORD dwStreamId);
	COutPin* GetOutPin(DWORD dwStreamId);

	CRITICAL_SECTION m_critSec;

	// https://docs.microsoft.com/en-us/windows/win32/medfound/media-event-generators
	CRITICAL_SECTION m_critSecForEventGenerator;


	IMFMediaEventQueue* m_pQueue;

	ComPtr<IMFTransform> m_spSourceTransform;

	HRESULT BridgeInputPinOutputPin(CInPin* pInPin, COutPin* pOutPin);
	HRESULT ChangeMediaTypeEx(ULONG pinId, IMFMediaType* pMediaType, DeviceStreamState newState);
	HRESULT SendEventToManager(MediaEventType, REFGUID, UINT32);
	HRESULT QueueEvent(IMFMediaEvent* pEvent);
	VOID SetStreamingState(DeviceStreamState state);

	DWORD m_dwMultithreadedWorkQueueId;
	LONG m_lWorkItemBasePriority;

	ComPtr<IKsControl> m_spIkscontrol;
	ComPtr<IUnknown> m_spDeviceManagerUnk;
	DeviceStreamState m_StreamingState;
};

class CMediaTypePrinter {
private:
	IMFMediaType* pMediaType;
	PCHAR m_pBuffer;
	ULONG buffLen;
public:
	CMediaTypePrinter(IMFMediaType* _pMediaType);
	~CMediaTypePrinter();
	STDMETHODIMP_(PCHAR) ToCompleteString();
	STDMETHODIMP_(PCHAR) ToString();
};

typedef MyMFT* PMyMFT;

