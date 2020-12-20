#pragma once

#include <mfidl.h>
#include <ks.h>
#include <ksproxy.h>

class MyMFT : public IMFRealTimeClientEx, public IKsControl, public IMFSampleAllocatorControl, public IMFDeviceTransform, public IMFShutdown, public IMFMediaEventGenerator,
	public IMFTransform
{
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
};

typedef MyMFT* PMyMFT;