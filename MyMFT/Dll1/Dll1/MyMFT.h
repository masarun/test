#pragma once

#include <mftransform.h>
#include <mfidl.h>
#include <ks.h>
#include <ksproxy.h>

class MyMFT : 
	public IMFDeviceTransform,
	public IMFTransform,
	public IMFShutdown,
	public IMFMediaEventGenerator,
	public IMFRealTimeClientEx,
	public IKsControl,
	public IMFSampleAllocatorControl
{
public:
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

private:
	ULONG m_cRef;
};
