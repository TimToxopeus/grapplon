#pragma once

#include "StateManager.h"

class CMenuState : public IStateManager
{
public:
	CMenuState();
	virtual ~CMenuState();

	virtual void Render();
	virtual void Update( float fTime );
	virtual bool HandleWiimoteEvent( wiimote_t* pWiimoteEvent );
	virtual int HandleSDLEvent( SDL_Event event );
};
