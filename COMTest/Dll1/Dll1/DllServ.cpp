#include "pch.h"
#include <iostream>
#include <Windows.h>
#include "Counts.h"
#include "Factory2.h"

using namespace std;

_Check_return_
STDAPI DllGetClassObject(_In_ REFCLSID rClsid, _In_ REFIID rIID, _Outptr_ VOID** ppInterface)
{
	HRESULT hr;
	PComFortuneTellerFactory pFactory;

	if (rClsid != CLSID_FortuneTeller)
	{
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

	return NOERROR;
}

__control_entrypoint(DllExport)
STDAPI  DllCanUnloadNow(void)
{
	return (((Counters::GetLockCount() == 0) && (Counters::GetObjectCount() == 0)) ? S_OK : S_FALSE);
}
