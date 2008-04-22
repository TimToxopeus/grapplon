#pragma once

#include <wiiuse.h>

class IWiimoteListener
{
public:
	virtual bool HandleWiimoteEvent(wiimote_t* pWiimoteEvent) = 0;
};
