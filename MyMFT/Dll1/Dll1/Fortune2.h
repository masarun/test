#pragma once

#include "IFortune.h"
//#include <Windows.h>
#include "Trace.h"

class ComFortuneTeller : public IFortuneTeller
{
public:
	// Constructor
	ComFortuneTeller();

	// IUknown
	STDMETHODIMP QueryInterface(REFIID, VOID**);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IFortuneTeller
	STDMETHOD(GetFortune)(BSTR*);

protected:
	ULONG m_cRef;
};

typedef ComFortuneTeller* PComFortuneTeller;
