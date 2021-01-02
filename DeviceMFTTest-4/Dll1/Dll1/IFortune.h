#pragma once

#include "GUIDs.h"

DECLARE_INTERFACE_(IFortuneTeller, IUnknown)
{
	STDMETHOD(GetFortune)(BSTR*) PURE;
};

typedef IFortuneTeller* PIFortuneTeller;
