#pragma once
#include "pch.h"
#include <iostream>
#include <Windows.h>
#include "Counts.h"
#include "Factory2.h"
#include "CRegistry.h"

_Check_return_
STDAPI DllRegisterServer();

_Check_return_
STDAPI DllUnregisterServer();

_Check_return_
STDAPI DllGetClassObject(_In_ REFCLSID rClsid, _In_ REFIID rIID, _Outptr_ VOID** ppInterface);

__control_entrypoint(DllExport)
STDAPI  DllCanUnloadNow(void);
