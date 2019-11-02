// Mft0.cpp : Implementation of CMft0

#include "stdafx.h"
#include "Mft0.h"
#include "SampleHelpers.h"
#include <WinString.h>

#include <samples/ocv_common.hpp>
#include <samples/slog.hpp>
#include "detectors.hpp"
#include <string>
#include <inference_engine.hpp>
#include <ie_iextension.h>
#include <ie_blob.h>
#include <ext_list.hpp>

using namespace InferenceEngine;
using namespace Extensions;
using namespace Cpu;


// CMft0
STDMETHODIMP CMft0::UpdateDsp(UINT32 uiPercentOfScreen)
{
    m_percentOfScreen = uiPercentOfScreen;
    return S_OK;
}

STDMETHODIMP CMft0::Enable(void)
{
    m_bEnableEffects = TRUE;
    return S_OK;
}

STDMETHODIMP CMft0::Disable(void)
{
    m_bEnableEffects = FALSE;
    return S_OK;
}

STDMETHODIMP CMft0::GetDspSetting(UINT* puiPercentOfScreen, BOOL* pIsEnabled)
{
    HRESULT hr = S_OK;
    do {
        CHK_NULL_PTR_BRK(puiPercentOfScreen);
        CHK_NULL_PTR_BRK(pIsEnabled);
        *puiPercentOfScreen = m_percentOfScreen;
        *pIsEnabled = m_bEnableEffects;
    } while (FALSE);

    return S_OK;
}

STDMETHODIMP CMft0::GetIids( 
    /* [out] */ _Out_ ULONG *iidCount,
    /* [size_is][size_is][out] */ _Outptr_result_buffer_maybenull_(*iidCount) IID **iids)
{
    HRESULT hr = S_OK;
    do {
        CHK_NULL_PTR_BRK(iidCount);
        CHK_NULL_PTR_BRK(iids);
        *iids = NULL;
        *iidCount = 0;
    } while (FALSE);

    return hr;
}

STDMETHODIMP CMft0::GetRuntimeClassName( 
    /* [out] */ _Outptr_result_maybenull_ HSTRING *className)
{
    HRESULT hr = S_OK;
    if(className != nullptr)
    {
        hr = WindowsCreateString(NULL, 0, className);
        if(FAILED(hr)){
            hr = E_OUTOFMEMORY;
        }
    }else {
        hr = E_INVALIDARG;
    }
    return  hr;
}

STDMETHODIMP CMft0::GetTrustLevel( 
    /* [out] */ _Out_ TrustLevel *trustLevel)
{
    HRESULT hr = S_OK;
    do {
        CHK_NULL_PTR_BRK(trustLevel);
        *trustLevel = TrustLevel::BaseTrust;
    } while (FALSE);

    return hr;
}


STDMETHODIMP CMft0::GetStreamLimits( 
    /* [out] */ _Out_ DWORD *pdwInputMinimum,
    /* [out] */ _Out_ DWORD *pdwInputMaximum,
    /* [out] */ _Out_ DWORD *pdwOutputMinimum,
    /* [out] */ _Out_ DWORD *pdwOutputMaximum)
{
    HRESULT hr = S_OK;
    do {
        if ((pdwInputMinimum == NULL) ||
            (pdwInputMaximum == NULL) ||
            (pdwOutputMinimum == NULL) ||
            (pdwOutputMaximum == NULL))
        {
            hr = E_POINTER;
            break;
        }

        // This MFT has a fixed number of streams.
        *pdwInputMinimum = 1;
        *pdwInputMaximum = 1;
        *pdwOutputMinimum = 1;
        *pdwOutputMaximum = 1;
    } while (FALSE);

    return hr;    
}


STDMETHODIMP CMft0::GetStreamCount( 
    /* [out] */ _Out_ DWORD *pcInputStreams,
    /* [out] */ _Out_ DWORD *pcOutputStreams)
{
    HRESULT hr = S_OK;

    do {
        if ((pcInputStreams == NULL) || (pcOutputStreams == NULL))

        {
            hr = E_POINTER;
            break;
        }

        // This MFT has a fixed number of streams.
        *pcInputStreams = 1;
        *pcOutputStreams = 1;
    } while (FALSE);

    return hr;
}
STDMETHODIMP CMft0::GetStreamIDs( 
    DWORD dwInputIDArraySize,
    /* [size_is][out] */ _Out_writes_(dwInputIDArraySize) DWORD *pdwInputIDs,
    DWORD dwOutputIDArraySize,
    /* [size_is][out] */ _Out_writes_(dwOutputIDArraySize) DWORD *pdwOutputIDs)
{
    dwOutputIDArraySize = 0;
    dwInputIDArraySize = 0;
    pdwInputIDs = NULL;
    pdwOutputIDs = NULL;
    return E_NOTIMPL; 
}

STDMETHODIMP CMft0::GetInputStreamInfo( 
    DWORD dwInputStreamID,
    /* [out] */ _Out_ MFT_INPUT_STREAM_INFO *pStreamInfo)
{
    HRESULT hr = S_OK;
    EnterCriticalSection(&m_critSec);
    
    do {
        CHK_NULL_BRK(pStreamInfo);
        if (!IsValidInputStream(dwInputStreamID))
        {
            CHK_LOG_BRK(MF_E_INVALIDSTREAMNUMBER);
        }
        if(m_pInputType) {
            pStreamInfo->cbAlignment = 0;
            pStreamInfo->cbSize = 0;
            pStreamInfo->dwFlags = MFT_INPUT_STREAM_WHOLE_SAMPLES | MFT_INPUT_STREAM_SINGLE_SAMPLE_PER_BUFFER ;
            pStreamInfo->hnsMaxLatency = 0;
        } else {
            hr = MF_E_TRANSFORM_TYPE_NOT_SET;
        }
    } while (FALSE);

    LeaveCriticalSection(&m_critSec);
    return hr;
}


STDMETHODIMP CMft0::GetOutputStreamInfo( 
    DWORD dwOutputStreamID,
    /* [out] */ _Out_ MFT_OUTPUT_STREAM_INFO *pStreamInfo)
{
    HRESULT hr = S_OK;
    EnterCriticalSection(&m_critSec);

    do {
        CHK_NULL_BRK(pStreamInfo);
        if (!IsValidInputStream(dwOutputStreamID))
        {
            CHK_LOG_BRK(MF_E_INVALIDSTREAMNUMBER);
        }
        if(m_pOutputType) {
            pStreamInfo->cbAlignment = 0;
            pStreamInfo->cbSize = 0;
            pStreamInfo->dwFlags = MFT_OUTPUT_STREAM_WHOLE_SAMPLES  | MFT_OUTPUT_STREAM_SINGLE_SAMPLE_PER_BUFFER | MFT_OUTPUT_STREAM_PROVIDES_SAMPLES | MFT_OUTPUT_STREAM_FIXED_SAMPLE_SIZE; 
        } else {
            hr = MF_E_TRANSFORM_TYPE_NOT_SET;
        }

    } while (FALSE);
    
    LeaveCriticalSection(&m_critSec);
    return hr;
}

STDMETHODIMP CMft0::GetAttributes( 
    /* [out] */ _Outptr_result_maybenull_ IMFAttributes **ppAttributes)
{
    HRESULT hr = S_OK;

    do {
        CHK_NULL_PTR_BRK(ppAttributes);
        if(!m_pGlobalAttributes) {
            CHK_LOG_BRK(MFCreateAttributes(&m_pGlobalAttributes, 3));
            CHK_LOG_BRK(m_pGlobalAttributes->SetUINT32(MF_TRANSFORM_ASYNC, FALSE));
            CHK_LOG_BRK(m_pGlobalAttributes->SetString(MFT_ENUM_HARDWARE_URL_Attribute, L"Sample_CameraExtensionMft"));
            CHK_LOG_BRK(m_pGlobalAttributes->SetUINT32(MFT_SUPPORT_DYNAMIC_FORMAT_CHANGE, TRUE));
        }
        *ppAttributes = m_pGlobalAttributes;
        (*ppAttributes)->AddRef();
    } while (FALSE);
    
    return hr;
}

STDMETHODIMP CMft0::GetInputStreamAttributes( 
    DWORD dwInputStreamID,
    /* [out] */ _Outptr_result_maybenull_ IMFAttributes **ppAttributes)
{
    HRESULT hr = S_OK;

    do {
        if(dwInputStreamID > 0) {
            hr = MF_E_INVALIDSTREAMNUMBER;
            break;
        }
        CHK_NULL_PTR_BRK(ppAttributes);
        if(!m_pInputAttributes){
            CHK_LOG_BRK(MFCreateAttributes(&m_pInputAttributes, 2));
            CHK_LOG_BRK(m_pInputAttributes->SetUINT32(MFT_SUPPORT_DYNAMIC_FORMAT_CHANGE, TRUE));
            CHK_LOG_BRK(m_pInputAttributes->SetString(MFT_ENUM_HARDWARE_URL_Attribute, L"Sample_CameraExtensionMft"));
        }
        *ppAttributes = m_pInputAttributes;
        (*ppAttributes)->AddRef();
    } while (FALSE);
    
    return hr;
}

STDMETHODIMP CMft0::GetOutputStreamAttributes( 
    DWORD dwOutputStreamID,
    /* [out] */ _Outptr_result_maybenull_ IMFAttributes **ppAttributes)
{
    HRESULT hr = S_OK;

    do {
        if(dwOutputStreamID > 0) {
            hr = MF_E_INVALIDSTREAMNUMBER;
            break;
        }
        CHK_NULL_PTR_BRK(ppAttributes);
        CHK_NULL_PTR_BRK(m_pInputAttributes);

        *ppAttributes = m_pInputAttributes;
        (*ppAttributes)->AddRef();
    } while (FALSE);
    
    return hr;
}

STDMETHODIMP CMft0::DeleteInputStream( 
    DWORD dwStreamID)
{
    HRESULT hr = S_OK;
    if(dwStreamID > 0) {
        hr = MF_E_INVALIDSTREAMNUMBER;
    } else {
        hr = E_NOTIMPL;
    }
    return hr; 
}

STDMETHODIMP CMft0::AddInputStreams( 
    DWORD cStreams,
    /* [in] */ _In_ DWORD *adwStreamIDs)
{
    HRESULT hr = S_OK;
    
    if( !adwStreamIDs || cStreams > 0) {
        hr = E_INVALIDARG;
    } else {
        hr = E_NOTIMPL;
    }
    return hr; 
}

STDMETHODIMP CMft0::GetInputAvailableType( 
    DWORD dwInputStreamID,
    DWORD dwTypeIndex,
    /* [out] */ _Outptr_result_maybenull_ IMFMediaType **ppType)
{
    HRESULT hr = S_OK;
    IUnknown *pUnk = NULL;
    IMFAttributes *pSourceAttributes = NULL;
    wchar_t *pszName;

    EnterCriticalSection(&m_critSec);

    do {
        CHK_BOOL_BRK(IsValidInputStream(dwInputStreamID));

        if(!m_pSourceTransform && m_pInputAttributes) {
            CHK_LOG_BRK(m_pInputAttributes->GetUnknown(MFT_CONNECTED_STREAM_ATTRIBUTE, IID_PPV_ARGS(&pSourceAttributes)));
            CHK_LOG_BRK(pSourceAttributes->GetUnknown(MF_DEVICESTREAM_EXTENSION_PLUGIN_CONNECTION_POINT, IID_PPV_ARGS(&pUnk)));
            CHK_LOG_BRK(pUnk->QueryInterface(__uuidof(IMFTransform), (void**)&m_pSourceTransform));
            CHK_LOG_BRK(pSourceAttributes->GetGUID( MF_DEVICESTREAM_STREAM_CATEGORY, &m_stStreamType));

            if(m_stStreamType == PINNAME_VIDEO_CAPTURE) {
                wprintf(L"Stream type: PINNAME_VIDEO_CAPTURE\n");
            } else if(m_stStreamType == PINNAME_VIDEO_PREVIEW) {
                wprintf(L"Stream type: PINNAME_VIDEO_PREVIEW\n");
            } else if(m_stStreamType == PINNAME_VIDEO_STILL) {
                wprintf(L"Stream type: PINNAME_VIDEO_STILL\n");
            } else if(m_stStreamType == PINNAME_IMAGE) {
                wprintf(L"Stream type: PINNAME_IMAGE\n");
            } else {
                StringFromCLSID(m_stStreamType, &pszName);
                if(pszName){
                    wprintf(L"Stream type: %s\n", pszName);
                    CoTaskMemFree(pszName);
                }
            }
            CHK_LOG_BRK((m_stStreamType == PINNAME_VIDEO_PREVIEW || m_stStreamType == PINNAME_VIDEO_CAPTURE) ? S_OK : E_UNEXPECTED);
            CHK_LOG_BRK(GenerateMFMediaTypeListFromDevice(dwInputStreamID));
        }

        CHK_LOG_BRK(GetMediaType(dwInputStreamID, dwTypeIndex, ppType));
    } while (FALSE);

    LeaveCriticalSection(&m_critSec);
    SAFERELEASE(pUnk);
    SAFERELEASE(pSourceAttributes);
    return hr;
}

STDMETHODIMP CMft0::GetOutputAvailableType( 
    DWORD dwOutputStreamID,
    DWORD dwTypeIndex,
    /* [out] */ _Outptr_result_maybenull_ IMFMediaType **ppType)
{
    HRESULT hr = S_OK;
    IUnknown *pUnk = NULL;
    IMFAttributes *pSourceAttributes = NULL;
    wchar_t *pszName;

    EnterCriticalSection(&m_critSec);

    do {
        CHK_BOOL_BRK(IsValidOutputStream(dwOutputStreamID));
        if(!m_pSourceTransform && m_pInputAttributes) {
            CHK_LOG_BRK(m_pInputAttributes->GetUnknown(MFT_CONNECTED_STREAM_ATTRIBUTE, IID_PPV_ARGS(&pSourceAttributes)));
            CHK_LOG_BRK(pSourceAttributes->GetUnknown(MF_DEVICESTREAM_EXTENSION_PLUGIN_CONNECTION_POINT, IID_PPV_ARGS(&pUnk)));
            CHK_LOG_BRK(pUnk->QueryInterface(__uuidof(IMFTransform), (void**)&m_pSourceTransform));
            CHK_LOG_BRK(pSourceAttributes->GetGUID( MF_DEVICESTREAM_STREAM_CATEGORY, &m_stStreamType));
            if(m_stStreamType == PINNAME_VIDEO_CAPTURE) {
                wprintf(L"Stream type: PINNAME_VIDEO_CAPTURE\n");
            } else if(m_stStreamType == PINNAME_VIDEO_PREVIEW) {
                wprintf(L"Stream type: PINNAME_VIDEO_PREVIEW\n");
            } else if(m_stStreamType == PINNAME_VIDEO_STILL) {
                wprintf(L"Stream type: PINNAME_VIDEO_STILL\n");
            } else if(m_stStreamType == PINNAME_IMAGE) {
                wprintf(L"Stream type: PINNAME_IMAGE\n");
            } else {
                StringFromCLSID(m_stStreamType, &pszName);
                wprintf(L"Stream type: %s\n", pszName);
            }
            CHK_LOG_BRK((m_stStreamType == PINNAME_VIDEO_PREVIEW || m_stStreamType == PINNAME_VIDEO_CAPTURE) ? S_OK : E_UNEXPECTED);
            CHK_LOG_BRK(GenerateMFMediaTypeListFromDevice(dwOutputStreamID));
        }

        CHK_LOG_BRK(GetMediaType(dwOutputStreamID, dwTypeIndex, ppType));
    } while (FALSE);
    
    LeaveCriticalSection(&m_critSec);
    SAFERELEASE(pUnk);
    SAFERELEASE(pSourceAttributes);
    return hr;
}

STDMETHODIMP CMft0::SetInputType( 
    DWORD dwInputStreamID,
    /* [in] */ _In_opt_ IMFMediaType *pType,
    DWORD dwFlags)
{
    HRESULT hr = S_OK;

    EnterCriticalSection(&m_critSec);

    BOOL bReallySet = ((dwFlags & MFT_SET_TYPE_TEST_ONLY) == 0);
    // Validate flags.
    do {
        if(bReallySet) {
            CComPtr<IMFMediaType> pFullType;
            CHK_LOG_BRK(IsMediaTypeSupported(dwInputStreamID, pType, &pFullType));
            SAFERELEASE(m_pInputType);
            SAFERELEASE(m_pOutputType);
            m_pOutputType = pFullType;
            m_pOutputType->AddRef();
            m_pInputType = pFullType;
            m_pInputType->AddRef();
        } else {
            CHK_LOG_BRK(IsMediaTypeSupported(dwInputStreamID, pType));
        }
    } while(FALSE);
    
    LeaveCriticalSection(&m_critSec);
    return hr;
}

STDMETHODIMP CMft0::SetOutputType( 
    DWORD dwOutputStreamID,
    /* [in] */ _In_opt_ IMFMediaType *pType,
    DWORD dwFlags)
{
    HRESULT hr = S_OK;

    do {
        CHK_LOG_BRK(SetInputType(dwOutputStreamID, pType, dwFlags));
    } while (FALSE);

    return hr;
}

STDMETHODIMP CMft0::GetInputCurrentType( 
    DWORD dwInputStreamID,
    /* [out] */ _Outptr_result_maybenull_ IMFMediaType **ppType)
{
    HRESULT hr = S_OK;

    EnterCriticalSection(&m_critSec);

    do {
        CHK_NULL_BRK(ppType);

        if (!IsValidInputStream(dwInputStreamID))
        {
            CHK_LOG_BRK(MF_E_INVALIDSTREAMNUMBER);
        }
        else if(m_pInputType)
        {
            *ppType = m_pInputType;
            (*ppType)->AddRef();
        }
        else 
        {
            CHK_LOG_BRK(MF_E_TRANSFORM_TYPE_NOT_SET);
        }
    } while (FALSE);

    LeaveCriticalSection(&m_critSec);
    return hr;
}

STDMETHODIMP CMft0::GetOutputCurrentType( 
    DWORD dwOutputStreamID,
    /* [out] */ _Outptr_result_maybenull_ IMFMediaType **ppType)
{
    HRESULT hr = S_OK;

    EnterCriticalSection(&m_critSec);

    do {
        CHK_NULL_BRK(ppType);

        if (!IsValidOutputStream(dwOutputStreamID))
        {
            CHK_LOG_BRK(MF_E_INVALIDSTREAMNUMBER);
        }
        else if(m_pOutputType) 
        {
            *ppType = m_pOutputType;
            (*ppType)->AddRef();
        }
        else
        {
            CHK_LOG_BRK(MF_E_TRANSFORM_TYPE_NOT_SET);
        }
    } while (FALSE);

    LeaveCriticalSection(&m_critSec);
    return hr;
}

STDMETHODIMP CMft0::GetInputStatus( 
    DWORD dwInputStreamID,
    /* [out] */ _Out_ DWORD *pdwFlags)
{
    HRESULT hr = S_OK;

    EnterCriticalSection(&m_critSec);

    do {
        CHK_BOOL_BRK(pdwFlags);

        if (!IsValidInputStream(dwInputStreamID))
        {
            CHK_LOG_BRK(MF_E_INVALIDSTREAMNUMBER);
        }

        // If we already have an input sample, we don't accept
        // another one until the client calls ProcessOutput or Flush.
        if (m_pSample == NULL)
        {
            *pdwFlags = MFT_INPUT_STATUS_ACCEPT_DATA;
        }
        else
        {
            *pdwFlags = 0;
        }
    } while (FALSE);
    
    LeaveCriticalSection(&m_critSec);
    return hr;
}

STDMETHODIMP CMft0::GetOutputStatus( 
    /* [out] */ _Out_ DWORD *pdwFlags)
{
    HRESULT hr = S_OK;

    EnterCriticalSection(&m_critSec);

    do {
        CHK_NULL_BRK(pdwFlags);
        // We can produce an output sample if (and only if)
        // we have an input sample.
        if (m_pSample != NULL)
        {
            *pdwFlags = MFT_OUTPUT_STATUS_SAMPLE_READY;
        }
        else
        {
            *pdwFlags = 0;
        }
    } while (FALSE);

    LeaveCriticalSection(&m_critSec);
    return hr;
}

STDMETHODIMP CMft0::SetOutputBounds( 
    LONGLONG hnsLowerBound,
    LONGLONG hnsUpperBound)
{
    UNREFERENCED_PARAMETER(hnsLowerBound);
    UNREFERENCED_PARAMETER(hnsUpperBound);
    return S_OK;
}

STDMETHODIMP CMft0::ProcessEvent( 
    DWORD dwInputStreamID,
    /* [in] */ _In_opt_ IMFMediaEvent *pEvent)
{
    UNREFERENCED_PARAMETER(dwInputStreamID);
    UNREFERENCED_PARAMETER(pEvent);
    return E_NOTIMPL;
}

STDMETHODIMP CMft0::ProcessMessage( 
    MFT_MESSAGE_TYPE eMessage,
    ULONG_PTR ulParam)
{
    UNREFERENCED_PARAMETER(ulParam);
    HRESULT hr = S_OK;

    EnterCriticalSection(&m_critSec);

    switch (eMessage)
    {
    case MFT_MESSAGE_COMMAND_FLUSH:
        // Flush the MFT.
        hr = OnFlush();
        break;

    case MFT_MESSAGE_COMMAND_DRAIN:
        // Drain: Tells the MFT not to accept any more input until
        // all of the pending output has been processed. That is our
        // default behevior already, so there is nothing to do.
        break;

    case MFT_MESSAGE_SET_D3D_MANAGER:
        // The pipeline should never send this message unless the MFT
        // has the MF_SA_D3D_AWARE attribute set to TRUE. However, if we
        // do get this message, it's invalid and we don't implement it.
        hr = E_NOTIMPL;
        break;

        // The remaining messages do not require any action from this MFT.
    case MFT_MESSAGE_NOTIFY_BEGIN_STREAMING:
    case MFT_MESSAGE_NOTIFY_END_STREAMING:
    case MFT_MESSAGE_NOTIFY_END_OF_STREAM:
    case MFT_MESSAGE_NOTIFY_START_OF_STREAM:
        break;
    }

    LeaveCriticalSection(&m_critSec);
    return hr;
}

STDMETHODIMP CMft0::ProcessInput( 
    DWORD dwInputStreamID,
    IMFSample *pSample,
    DWORD dwFlags)
{
    HRESULT hr = S_OK;

    EnterCriticalSection(&m_critSec);

    do {
        CHK_NULL_BRK(pSample);

        CHK_BOOL_BRK(dwFlags == 0)

            DWORD dwBufferCount = 0;

        if (!IsValidInputStream(dwInputStreamID))
        {
            CHK_LOG_BRK(hr = MF_E_INVALIDSTREAMNUMBER);
        }

        if (!m_pInputType || !m_pOutputType)
        {
            CHK_LOG_BRK(MF_E_NOTACCEPTING);   // Client must set input and output types.
        }

        if (m_pSample != NULL)
        {
            CHK_LOG_BRK(MF_E_NOTACCEPTING);   // We already have an input sample.
        }

        // Validate the number of buffers. There should only be a single buffer to hold the video frame.
        CHK_LOG_BRK(pSample->GetBufferCount(&dwBufferCount));

        if (dwBufferCount == 0)
        {
            CHK_LOG_BRK(E_FAIL);
        }
        if (dwBufferCount > 1)
        {
            CHK_LOG_BRK(MF_E_SAMPLE_HAS_TOO_MANY_BUFFERS);
        }

        // Cache the sample. We do the actual work in ProcessOutput.
        m_pSample = pSample;
        pSample->AddRef();  // Hold a reference count on the sample.
    } while (FALSE);

    LeaveCriticalSection(&m_critSec);
    return hr;
}

STDMETHODIMP CMft0::ProcessOutput( 
    DWORD dwFlags,
    DWORD cOutputBufferCount,
    /* [size_is][out][in] */ MFT_OUTPUT_DATA_BUFFER *pOutputSamples,
    /* [out] */ DWORD *pdwStatus)
{
    HRESULT hr = S_OK;

    IMFMediaBuffer *pMediaBufInput = NULL,
        *pMediaBufOutput = NULL;
    IMFSample *pOutputIMFSample = NULL;
    LONGLONG hnsDuration = 0,
        hnsTime = 0;

    EnterCriticalSection(&m_critSec);

    do {
        CHK_BOOL_BRK(dwFlags == 0);
        CHK_NULL_BRK(pOutputSamples);
        CHK_NULL_BRK(pdwStatus);

        // Must be exactly one output buffer.
        CHK_BOOL_BRK(cOutputBufferCount == 1);

        // If we don't have an input sample, we need some input before
        // we can generate any output.
        if (m_pSample == NULL)
        {
            hr = MF_E_TRANSFORM_NEED_MORE_INPUT;
            break;
        }

        CHK_LOG_BRK(m_pSample->ConvertToContiguousBuffer(&pMediaBufInput));

        MFCreateSample(&pOutputIMFSample);

        if(!pOutputSamples[0].pSample){
            CHK_LOG_BRK(m_pSample->ConvertToContiguousBuffer(&pMediaBufOutput));

        } else {
            CHK_LOG_BRK(pOutputSamples[0].pSample->ConvertToContiguousBuffer(&pMediaBufOutput));
        }

        pOutputIMFSample->AddBuffer(pMediaBufOutput);

        CHK_LOG_BRK(OnProcessOutput(pMediaBufInput, pMediaBufOutput));
        pOutputSamples[0].pSample  =  pOutputIMFSample;
        pOutputIMFSample->AddRef();

        pOutputSamples[0].dwStatus = 0;
        *pdwStatus = 0;
        if (SUCCEEDED(m_pSample->GetSampleDuration(&hnsDuration)))
        {
            CHK_LOG_BRK(pOutputSamples[0].pSample->SetSampleDuration(hnsDuration));
        }

        if (SUCCEEDED(m_pSample->GetSampleTime(&hnsTime)))
        {
            CHK_LOG_BRK(pOutputSamples[0].pSample->SetSampleTime(hnsTime));
        }
    } while (FALSE);

    SAFERELEASE(pOutputIMFSample);
    SAFERELEASE(pMediaBufInput);
    SAFERELEASE(pMediaBufOutput);
    SAFERELEASE(m_pSample);   // Release our input sample.
    LeaveCriticalSection(&m_critSec);
    return hr;
}

BOOL CMft0::IsValidInputStream(DWORD dwInputStreamID)
{
    return dwInputStreamID == 0;
}

// IsValidOutputStream: Returns TRUE if dwOutputStreamID is a valid output stream identifier.
BOOL CMft0::IsValidOutputStream(DWORD dwOutputStreamID)
{
    //update
    return dwOutputStreamID == 0;
}


STDMETHODIMP CMft0::OnProcessOutput(IMFMediaBuffer *pIn, IMFMediaBuffer *pOut)
{
    HRESULT hr = S_OK;
    LONG lDefaultStride = 0,
        lSrcStride = 0,
        lDestStride = 0;
    UINT uiWidth = 0,
         uiHeight = 0;
    BYTE *pDest = NULL,
        *pSrc = NULL;
    //BOOL bCompressed = TRUE;
    GUID stSubType = {0};

    do {
        CHK_NULL_PTR_BRK(m_pSample);
        CHK_NULL_PTR_BRK(m_pInputType);

        CHK_LOG_BRK(m_pInputType->GetGUID(MF_MT_SUBTYPE, &stSubType));

        if(((stSubType == MFVideoFormat_RGB8) || (stSubType == MFVideoFormat_RGB555) ||
            (stSubType == MFVideoFormat_RGB565) || (stSubType == MFVideoFormat_RGB24) ||
            (stSubType == MFVideoFormat_RGB32)  || (stSubType == MFVideoFormat_ARGB32) ||
            (stSubType == MFVideoFormat_AI44)   || (stSubType == MFVideoFormat_AYUV) ||
            (stSubType == MFVideoFormat_I420)   || (stSubType == MFVideoFormat_IYUV) || 
            (stSubType == MFVideoFormat_NV11)   || (stSubType == MFVideoFormat_NV12) ||
            (stSubType == MFVideoFormat_UYVY)   || (stSubType == MFVideoFormat_Y41P) ||
            (stSubType == MFVideoFormat_Y41T)   || (stSubType == MFVideoFormat_Y42T) ||
            (stSubType == MFVideoFormat_YUY2)   || (stSubType == MFVideoFormat_YV12) ||
            (stSubType == MFVideoFormat_P010)   || (stSubType == MFVideoFormat_P016) ||
            (stSubType == MFVideoFormat_P210)   || (stSubType == MFVideoFormat_P216) ||
            (stSubType == MFVideoFormat_v210)   || (stSubType == MFVideoFormat_v216) ||
            (stSubType == MFVideoFormat_v410)   || (stSubType == MFVideoFormat_Y210) ||
            (stSubType == MFVideoFormat_Y216)   || (stSubType == MFVideoFormat_Y410) ||
            (stSubType == MFVideoFormat_Y416))  && m_bEnableEffects)
        {
            CHK_LOG_BRK(GetDefaultStride(&lDefaultStride));
            CHK_LOG_BRK(MFGetAttributeSize(m_pInputType, MF_MT_FRAME_SIZE, &uiWidth, &uiHeight));
            VideoBufferLock inputLock(pIn);
            VideoBufferLock outputLock(pOut);

            // Lock the input buffer.
            CHK_LOG_BRK(inputLock.LockBuffer(lDefaultStride, uiHeight, &pSrc, &lSrcStride));

			HKEY hKey;
			if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\Temp", NULL, KEY_READ, &hKey) == ERROR_SUCCESS)
			{

				DWORD value = 0;
				DWORD size = sizeof(value);
				DWORD type;
				if (RegQueryValueExW(hKey, L"Debug", 0, &type, (LPBYTE)&value, &size) == ERROR_SUCCESS)
				{
					if (value != 0)
					{
						// https://docs.opencv.org/3.4/d3/d63/classcv_1_1Mat.html#a51615ebf17a64c968df0bf49b4de6a3a
						cv::Mat frame(720, 1280, CV_8UC3, pSrc);

						//pathToModel "c:\\Users\\masarun\\Downloads\\OpenVINO\\face-detection-adas-0001.xml"
						//deviceForInference = "CPU"
						//maxBatch = 1
						//isBatchDynamic = false
						//isAsync = false
						//detectionThreshold = 0.50000000000000000
						//doRawOutputMessages = false
						//bb_enlarge_coefficient = 1.20000005
						//bb_dx_coefficient = 1.00000000
						//bb_dy_coefficient = 1.00000000

						Core ie;

						FaceDetection faceDetector(
							//std::string("c:\\Users\\masarun\\Downloads\\OpenVINO\\face-detection-adas-0001.xml"), 
							std::string("C:\\Program Files\\SampleMft0\\face-detection-adas-0001.xml"),
							std::string("CPU"), 
							1, 
							false, 
							false, 
							0.50000000000000000, 
							false,
							(float)1.20000005, (float)1.00000000, (float)1.00000000);
						
						//ie.AddExtension(std::make_shared<Extensions::Cpu::CpuExtensions>(), "CPU");

						Load(faceDetector).into(ie, std::string("CPU"), false);

						faceDetector.enqueue(frame);
						faceDetector.submitRequest();

						faceDetector.wait();
						faceDetector.fetchResults();
					}
				}

				RegCloseKey(hKey);
			}


            // Lock the output buffer.
            CHK_LOG_BRK(outputLock.LockBuffer(lDefaultStride, uiHeight, &pDest, &lDestStride));
            long lines =  uiHeight;
            if(m_percentOfScreen != -1 && m_percentOfScreen != 0) {
                lines = (UINT)(uiHeight * (1.0- m_percentOfScreen/100.00));
            }
            for(long i = 0; i < (long)uiHeight; i++) {
                if(lDestStride < 0) {
                    if(i >= lines) {
						// Fill the byte of the line with 128 (0x80).
						// So, the line is filled by 128, 128, 128, 128, 128, ...
						// RGB (128, 128, 128) -> Gray
						//memset(pDest+i*lDestStride, 128, lDestStride);

						// Fill the byte of the line with 0x00.
						// Why the line filled by 0x00 is displayed in green?
                        memset(pDest+i*lDestStride, 0, abs(lDefaultStride));
                    } else {
                        memcpy(pDest+i*lDestStride, pSrc+i*lDefaultStride, abs(lDefaultStride));
                    }
                } else {
                    if(i >= lines) {
                        memset(pDest+i*lDestStride, 0, lDestStride);
                    } else {
                        memcpy(pDest+i*lDestStride, pSrc+i*lDestStride, lDestStride);
                    }
                }
            }
        } 
    } while (FALSE);

    return hr;
}

STDMETHODIMP CMft0::GetDefaultStride(LONG *plStride)
{
    LONG lStride = 0;

    // Try to get the default stride from the media type.
    HRESULT hr = m_pInputType->GetUINT32(MF_MT_DEFAULT_STRIDE, (UINT32*)&lStride);
    if (FAILED(hr))
    {
        // Attribute not set. Try to calculate the default stride.
        GUID subtype = GUID_NULL;
        UINT32 width = 0;
        UINT32 height = 0;

        // Get the subtype and the image size.
        hr = m_pInputType->GetGUID(MF_MT_SUBTYPE, &subtype);
        if (SUCCEEDED(hr))
        {
            hr = MFGetAttributeSize(m_pInputType, MF_MT_FRAME_SIZE, &width, &height);
        }
        if (SUCCEEDED(hr))
        {
            hr = MFGetStrideForBitmapInfoHeader(subtype.Data1, width, &lStride);
        }

        // Set the attribute for later reference.
        if (SUCCEEDED(hr))
        {
            (void)m_pInputType->SetUINT32(MF_MT_DEFAULT_STRIDE, UINT32(lStride));
        }
    }

    if (SUCCEEDED(hr))
    {
        *plStride = lStride;
    }

    return hr;
}

STDMETHODIMP CMft0::OnFlush()
{
    HRESULT hr = S_OK;

    // For this MFT, flushing just means releasing the input sample.
    SAFERELEASE(m_pSample);

    return hr;
}

STDMETHODIMP CMft0::GetMediaType( DWORD dwStreamId, DWORD dwTypeIndex, IMFMediaType **ppType)
{
    HRESULT hr = S_OK;

    do {
        CHK_NULL_PTR_BRK(ppType);
        if(dwStreamId != 0)
            CHK_LOG_BRK(MF_E_INVALIDSTREAMNUMBER);
        if(dwTypeIndex >= m_listOfMediaTypes.GetCount())
            CHK_LOG_BRK(MF_E_NO_MORE_TYPES);

        *ppType = m_listOfMediaTypes[dwTypeIndex];
        (*ppType)->AddRef();
    } while (FALSE);

    return hr;
}

STDMETHODIMP CMft0::IsMediaTypeSupported(UINT uiStreamId, IMFMediaType *pIMFMediaType, IMFMediaType **ppIMFMediaTypeFull)
{
    HRESULT hr = S_OK;

    do {
        CHK_NULL_PTR_BRK(pIMFMediaType);

        if(uiStreamId != 0)
        {
            CHK_LOG_BRK(MF_E_INVALIDINDEX);
        }
        BOOL bFound =FALSE;
        for(UINT i = 0; i< m_listOfMediaTypes.GetCount(); i++)
        {
            DWORD   dwResult = 0;
            hr = m_listOfMediaTypes[i]->IsEqual(pIMFMediaType, &dwResult);
            if(hr == S_FALSE)
            {

                if((dwResult & MF_MEDIATYPE_EQUAL_MAJOR_TYPES) && 
                    (dwResult& MF_MEDIATYPE_EQUAL_FORMAT_TYPES) && 
                    (dwResult& MF_MEDIATYPE_EQUAL_FORMAT_DATA))
                {
                    hr = S_OK;
                }
            }
            if(hr == S_OK)
            {
                bFound = TRUE;
                if(ppIMFMediaTypeFull) {
                    *ppIMFMediaTypeFull = m_listOfMediaTypes[i];
                    (*ppIMFMediaTypeFull)->AddRef();
                }
                break;
            }
            else if(FAILED(hr))
            {
                CHK_LOG_BRK(hr);
            }

        }
        if(bFound == FALSE)
        {
            CHK_LOG_BRK(MF_E_INVALIDMEDIATYPE);
        }
    } while (FALSE);

    return hr;
}

STDMETHODIMP CMft0::GenerateMFMediaTypeListFromDevice(UINT uiStreamId)
{
    HRESULT hr = S_OK;
    GUID stSubType = {0};

    do {
        CHK_NULL_PTR_BRK(m_pSourceTransform);

        m_listOfMediaTypes.RemoveAll();
        for(UINT iMediaType = 0; TRUE; iMediaType++) 
        {
            CComPtr<IMFMediaType> pMediaType;
            hr = m_pSourceTransform->GetOutputAvailableType(uiStreamId, iMediaType, &pMediaType);
            if(hr != S_OK)
                break;
            CHK_LOG_BRK(pMediaType->GetGUID(MF_MT_SUBTYPE, &stSubType));

            if(((stSubType == MFVideoFormat_RGB8)   || (stSubType == MFVideoFormat_RGB555) ||
                (stSubType == MFVideoFormat_RGB565) || (stSubType == MFVideoFormat_RGB24) ||
                (stSubType == MFVideoFormat_RGB32)  || (stSubType == MFVideoFormat_ARGB32) ||
                (stSubType == MFVideoFormat_AI44)   || (stSubType == MFVideoFormat_AYUV) ||
                (stSubType == MFVideoFormat_I420)   || (stSubType == MFVideoFormat_IYUV) || 
                (stSubType == MFVideoFormat_NV11)   || (stSubType == MFVideoFormat_NV12) ||
                (stSubType == MFVideoFormat_UYVY)   || (stSubType == MFVideoFormat_Y41P) ||
                (stSubType == MFVideoFormat_Y41T)   || (stSubType == MFVideoFormat_Y42T) ||
                (stSubType == MFVideoFormat_YUY2)   || (stSubType == MFVideoFormat_YV12) ||
                (stSubType == MFVideoFormat_P010)   || (stSubType == MFVideoFormat_P016) ||
                (stSubType == MFVideoFormat_P210)   || (stSubType == MFVideoFormat_P216) ||
                (stSubType == MFVideoFormat_v210)   || (stSubType == MFVideoFormat_v216) ||
                (stSubType == MFVideoFormat_v410)   || (stSubType == MFVideoFormat_Y210) ||
                (stSubType == MFVideoFormat_Y216)   || (stSubType == MFVideoFormat_Y410) ||
                (stSubType == MFVideoFormat_Y416))  && m_bEnableEffects)
            {
                m_listOfMediaTypes[(ULONG)(m_listOfMediaTypes.GetCount())] = pMediaType;
            }
        }        

    } while (FALSE);

    if(hr == MF_E_NO_MORE_TYPES) {
        hr = S_OK;
    }

    return hr;
}


BaseDetection::BaseDetection(std::string topoName,
	const std::string& pathToModel,
	const std::string& deviceForInference,
	int maxBatch, bool isBatchDynamic, bool isAsync,
	bool doRawOutputMessages)
	: topoName(topoName), pathToModel(pathToModel), deviceForInference(deviceForInference),
	maxBatch(maxBatch), isBatchDynamic(isBatchDynamic), isAsync(isAsync),
	enablingChecked(false), _enabled(false), doRawOutputMessages(doRawOutputMessages) {
	if (isAsync) {
		slog::info << "Use async mode for " << topoName << slog::endl;
	}
}

BaseDetection::~BaseDetection() {}

ExecutableNetwork* BaseDetection::operator ->() {
	return &net;
}

void BaseDetection::submitRequest() {
	if (!enabled() || request == nullptr) return;
	if (isAsync) {
		request->StartAsync();
	}
	else {
		request->Infer();
	}
}

void BaseDetection::wait() {
	if (!enabled() || !request || !isAsync)
		return;
	request->Wait(IInferRequest::WaitMode::RESULT_READY);
}

bool BaseDetection::enabled() const {
	if (!enablingChecked) {
		_enabled = !pathToModel.empty();
		if (!_enabled) {
			slog::info << topoName << " DISABLED" << slog::endl;
		}
		enablingChecked = true;
	}
	return _enabled;
}

void BaseDetection::printPerformanceCounts(std::string fullDeviceName) {
	if (!enabled()) {
		return;
	}
	slog::info << "Performance counts for " << topoName << slog::endl << slog::endl;
	::printPerformanceCounts(*request, std::cout, fullDeviceName, false);
}


FaceDetection::FaceDetection(const std::string& pathToModel,
	const std::string& deviceForInference,
	int maxBatch, bool isBatchDynamic, bool isAsync,
	double detectionThreshold, bool doRawOutputMessages,
	float bb_enlarge_coefficient, float bb_dx_coefficient, float bb_dy_coefficient)
	: BaseDetection("Face Detection", pathToModel, deviceForInference, maxBatch, isBatchDynamic, isAsync, doRawOutputMessages),
	detectionThreshold(detectionThreshold),
	maxProposalCount(0), objectSize(0), enquedFrames(0), width(0), height(0),
	bb_enlarge_coefficient(bb_enlarge_coefficient), bb_dx_coefficient(bb_dx_coefficient),
	bb_dy_coefficient(bb_dy_coefficient), resultsFetched(false) 
{
}

void FaceDetection::submitRequest() {
	if (!enquedFrames) return;
	enquedFrames = 0;
	resultsFetched = false;
	results.clear();
	BaseDetection::submitRequest();
}

void FaceDetection::enqueue(const cv::Mat& frame) {
	if (!enabled()) return;

	if (!request) {
		request = net.CreateInferRequestPtr();
	}

	width = static_cast<float>(frame.cols);
	height = static_cast<float>(frame.rows);

	Blob::Ptr  inputBlob = request->GetBlob(input);

	matU8ToBlob<uint8_t>(frame, inputBlob);

	enquedFrames = 1;
}

CNNNetwork FaceDetection::read() {
	slog::info << "Loading network files for Face Detection" << slog::endl;
	CNNNetReader netReader;
	/** Read network model **/
	netReader.ReadNetwork(pathToModel);
	/** Set batch size to 1 **/
	slog::info << "Batch size is set to " << maxBatch << slog::endl;
	netReader.getNetwork().setBatchSize(maxBatch);
	/** Extract model name and load its weights **/
	std::string binFileName = fileNameNoExt(pathToModel) + ".bin";
	netReader.ReadWeights(binFileName);
	/** Read labels (if any)**/
	std::string labelFileName = fileNameNoExt(pathToModel) + ".labels";

	std::ifstream inputFile(labelFileName);
	std::copy(std::istream_iterator<std::string>(inputFile),
		std::istream_iterator<std::string>(),
		std::back_inserter(labels));
	// -----------------------------------------------------------------------------------------------------

	/** SSD-based network should have one input and one output **/
	// ---------------------------Check inputs -------------------------------------------------------------
	slog::info << "Checking Face Detection network inputs" << slog::endl;
	InputsDataMap inputInfo(netReader.getNetwork().getInputsInfo());
	if (inputInfo.size() != 1) {
		throw std::logic_error("Face Detection network should have only one input");
	}
	InputInfo::Ptr inputInfoFirst = inputInfo.begin()->second;
	inputInfoFirst->setPrecision(Precision::U8);
	// -----------------------------------------------------------------------------------------------------

	// ---------------------------Check outputs ------------------------------------------------------------
	slog::info << "Checking Face Detection network outputs" << slog::endl;
	OutputsDataMap outputInfo(netReader.getNetwork().getOutputsInfo());
	if (outputInfo.size() != 1) {
		throw std::logic_error("Face Detection network should have only one output");
	}
	DataPtr& _output = outputInfo.begin()->second;
	output = outputInfo.begin()->first;

	const CNNLayerPtr outputLayer = netReader.getNetwork().getLayerByName(output.c_str());
	if (outputLayer->type != "DetectionOutput") {
		throw std::logic_error("Face Detection network output layer(" + outputLayer->name +
			") should be DetectionOutput, but was " + outputLayer->type);
	}

	if (outputLayer->params.find("num_classes") == outputLayer->params.end()) {
		throw std::logic_error("Face Detection network output layer (" +
			output + ") should have num_classes integer attribute");
	}

	const size_t num_classes = outputLayer->GetParamAsUInt("num_classes");
	if (labels.size() != num_classes) {
		if (labels.size() == (num_classes - 1))  // if network assumes default "background" class, which has no label
			labels.insert(labels.begin(), "fake");
		else
			labels.clear();
	}
	const SizeVector outputDims = _output->getTensorDesc().getDims();
	maxProposalCount = outputDims[2];
	objectSize = outputDims[3];
	if (objectSize != 7) {
		throw std::logic_error("Face Detection network output layer should have 7 as a last dimension");
	}
	if (outputDims.size() != 4) {
		throw std::logic_error("Face Detection network output dimensions not compatible shoulld be 4, but was " +
			std::to_string(outputDims.size()));
	}
	_output->setPrecision(Precision::FP32);

	slog::info << "Loading Face Detection model to the " << deviceForInference << " device" << slog::endl;
	input = inputInfo.begin()->first;
	return netReader.getNetwork();
}

void FaceDetection::fetchResults() {
	if (!enabled()) return;
	results.clear();
	if (resultsFetched) return;
	resultsFetched = true;
	const float* detections = request->GetBlob(output)->buffer().as<float*>();

	for (int i = 0; i < maxProposalCount; i++) {
		float image_id = detections[i * objectSize + 0];
		if (image_id < 0) {
			break;
		}
		Result r;
		r.label = static_cast<int>(detections[i * objectSize + 1]);
		r.confidence = detections[i * objectSize + 2];

		if (r.confidence <= detectionThreshold && !doRawOutputMessages) {
			continue;
		}

		r.location.x = static_cast<int>(detections[i * objectSize + 3] * width);
		r.location.y = static_cast<int>(detections[i * objectSize + 4] * height);
		r.location.width = static_cast<int>(detections[i * objectSize + 5] * width - r.location.x);
		r.location.height = static_cast<int>(detections[i * objectSize + 6] * height - r.location.y);

		// Make square and enlarge face bounding box for more robust operation of face analytics networks
		int bb_width = r.location.width;
		int bb_height = r.location.height;

		int bb_center_x = r.location.x + bb_width / 2;
		int bb_center_y = r.location.y + bb_height / 2;

		int max_of_sizes = std::max(bb_width, bb_height);

		int bb_new_width = static_cast<int>(bb_enlarge_coefficient * max_of_sizes);
		int bb_new_height = static_cast<int>(bb_enlarge_coefficient * max_of_sizes);

		r.location.x = bb_center_x - static_cast<int>(std::floor(bb_dx_coefficient * bb_new_width / 2));
		r.location.y = bb_center_y - static_cast<int>(std::floor(bb_dy_coefficient * bb_new_height / 2));

		r.location.width = bb_new_width;
		r.location.height = bb_new_height;

		if (doRawOutputMessages) {
			std::cout << "[" << i << "," << r.label << "] element, prob = " << r.confidence <<
				"    (" << r.location.x << "," << r.location.y << ")-(" << r.location.width << ","
				<< r.location.height << ")"
				<< ((r.confidence > detectionThreshold) ? " WILL BE RENDERED!" : "") << std::endl;
		}
		if (r.confidence > detectionThreshold) {
			results.push_back(r);
		}
	}
}

Load::Load(BaseDetection& detector) : detector(detector) {
}

void Load::into(InferenceEngine::Core& ie, const std::string& deviceName, bool enable_dynamic_batch) const {
	if (detector.enabled()) {
		std::map<std::string, std::string> config = { };
		bool isPossibleDynBatch = deviceName.find("CPU") != std::string::npos ||
			deviceName.find("GPU") != std::string::npos;

		if (enable_dynamic_batch && isPossibleDynBatch) {
			config[PluginConfigParams::KEY_DYN_BATCH_ENABLED] = PluginConfigParams::YES;
		}

		detector.net = ie.LoadNetwork(detector.read(), deviceName, config);
	}
}
