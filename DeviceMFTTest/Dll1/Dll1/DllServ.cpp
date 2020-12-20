#include "pch.h"
#include "DllServ.tmh"
#include "DllServ.h"

#define CLSID_PATH L"CLSID\\" GUID_Hoge

using namespace std;

bool AddRegistryValue(LPCTSTR szSubKey, LPCTSTR szValueName, LPCTSTR szValue);

bool GetModulePath(TCHAR* szPath, DWORD cchPath)
{
    bool result = false;
    HMODULE hModule;

    if (szPath == NULL)
    {
        return result;
    }

    // https://stackoverflow.com/questions/6924195/get-dll-path-at-runtime
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)&AddRegistryValue, &hModule) != 0)
    {
        if (GetModuleFileName(hModule, (LPWSTR)szPath, cchPath) != 0)
        {
            result = true;
        }
    }

    return result;
}

bool AddRegistryValue(LPCTSTR szSubKey, LPCTSTR szValueName, LPCTSTR szValue)
{
    bool result = false;

    if ((szSubKey == NULL) || (lstrlen(szSubKey) == 0) || (szValue == NULL) || (lstrlen(szValue) == 0))
    {
        return result;
    }

    CRegistry registry(HKEY_CLASSES_ROOT);

    result = registry.CreateOrOpenSubKey(szSubKey);
    if (result)
    {
        result = registry.SetStringValue(szValueName, szValue);
    }

    return result;
}

_Check_return_
STDAPI DllRegisterServer()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "DllRegisterServer Entry");

    HRESULT hr = E_FAIL;
    bool result = false;

    TCHAR szPath[MAX_PATH] = { 0 };
    DWORD cchPath = MAX_PATH;
    result = GetModulePath(szPath, cchPath);

    if (!result)
    {
        return hr;
    }

    result = AddRegistryValue(CLSID_PATH L"\\InprocServer32", L"ThreadingModel", L"Both");
    if (result)
    {
        result = AddRegistryValue(CLSID_PATH L"\\InprocServer32", NULL, szPath);
        if (result)
        {
            result = AddRegistryValue(CLSID_PATH, NULL, L"FortuneTeller");
        }
    }

    if (result)
    {
        hr = S_OK;
    }
    else
    {
        CRegistry registry(HKEY_CLASSES_ROOT);
        registry.DeleteSubKey(CLSID_PATH);
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "DllRegisterServer Exit");

    return hr;
}

_Check_return_
STDAPI DllUnregisterServer()
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "DllUnregisterServer Entry");

    HRESULT hr = E_FAIL;

    CRegistry registry(HKEY_CLASSES_ROOT);
    if (registry.DeleteSubKey(CLSID_PATH))
    {
        hr = S_OK;
    }

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "DllUnregisterServer Exit");

    return hr;
}


_Check_return_
STDAPI DllGetClassObject(_In_ REFCLSID rClsid, _In_ REFIID rIID, _Outptr_ VOID** ppInterface)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "DllGetClassObject Entry");

	HRESULT hr;
	PComFortuneTellerFactory pFactory;

	if (rClsid != CLSID_FortuneTeller)
	{
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "DllGetClassObject: rClsid != CLSID_FortuneTeller");

		return E_FAIL;
	}

	pFactory = new ComFortuneTellerFactory();

	if (!pFactory)
	{
		return E_OUTOFMEMORY;
	}

	hr = pFactory->QueryInterface(rIID, ppInterface);
	if (FAILED(hr))
	{
		delete pFactory;
		return hr;
	}

    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "DllGetClassObject Exit");
	return NOERROR;
}

__control_entrypoint(DllExport)
STDAPI  DllCanUnloadNow(void)
{
    TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "DllCanUnloadNow");

	return (((Counters::GetLockCount() == 0) && (Counters::GetObjectCount() == 0)) ? S_OK : S_FALSE);
}
