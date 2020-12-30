#pragma once

#include <Windows.h>
#include <combaseapi.h>
#include <ks.h>
#include <comdef.h>

#include <stdexcept>



#define SAFERELEASE(x) \
if (x) {\
    x->Release(); \
    x = NULL; \
}

#if !defined(_IKsControl_)
#define _IKsControl_
interface DECLSPEC_UUID("28F54685-06FD-11D2-B27A-00A0C9223196") IKsControl;
#undef INTERFACE
#define INTERFACE IKsControl
DECLARE_INTERFACE_(IKsControl, IUnknown)
{
    STDMETHOD(KsProperty)(
        THIS_
        IN PKSPROPERTY Property,
        IN ULONG PropertyLength,
        IN OUT LPVOID PropertyData,
        IN ULONG DataLength,
        OUT ULONG * BytesReturned
        ) PURE;
    STDMETHOD(KsMethod)(
        THIS_
        IN PKSMETHOD Method,
        IN ULONG MethodLength,
        IN OUT LPVOID MethodData,
        IN ULONG DataLength,
        OUT ULONG * BytesReturned
        ) PURE;
    STDMETHOD(KsEvent)(
        THIS_
        IN PKSEVENT Event OPTIONAL,
        IN ULONG EventLength,
        IN OUT LPVOID EventData,
        IN ULONG DataLength,
        OUT ULONG * BytesReturned
        ) PURE;
};
#endif //!defined(_IKsControl_)

#define _DEFINE_DEVICEMFT_MFT0HELPER_IMPL__    \
STDMETHOD(MFTGetStreamCount)(\
    _Inout_  DWORD* pdwInputStreams, \
    _Inout_  DWORD* pdwOutputStreams        \
    )                                          \
{                                              \
    return GetStreamCount(pdwInputStreams, pdwOutputStreams);  \
}

#define SAFE_DELETE(p)              delete p; p = NULL;

#define MF_DEVICEMFT_ASYNCPIN_NEEDED           1

#if !defined DMFTCHECKHR_GOTO
#define DMFTCHECKHR_GOTO(a,b) {hr=(a); if(FAILED(hr)){goto b;}} 
#endif

#if !defined DMFTCHECKNULL_GOTO
#define DMFTCHECKNULL_GOTO(a,b,c) {if(!a) {hr = c; goto b;}} 
#endif

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

