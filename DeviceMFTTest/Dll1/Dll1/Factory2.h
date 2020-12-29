#pragma once

#include "IFortune.h"

class ComFortuneTellerFactory : public IClassFactory
{
public:
	// Constructor
	ComFortuneTellerFactory();

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID, VOID**);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IClassFactory
	STDMETHODIMP CreateInstance(LPUNKNOWN, REFIID, VOID**);
	STDMETHODIMP LockServer(BOOL);

protected:
	ULONG m_cRef;
};

typedef ComFortuneTellerFactory* PComFortuneTellerFactory;
