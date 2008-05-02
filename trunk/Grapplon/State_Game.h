#pragma once

#include "StateManager.h"
#include <sdl.h>

// Forward declaration
class CAnimatedTexture;
class CPlayerObject;
class CUniverse;

class CGameState : public IStateManager
{
private:
	SDL_Rect box;
	CPlayerObject *m_pPlayers[4];
	CUniverse *m_pUniverse;

	CAnimatedTexture *m_pSpace;

public:
	CGameState();
	virtual ~CGameState();

	virtual void Render();
	virtual void Update( float fTime );
	virtual bool HandleWiimoteEvent( wiimote_t* pWiimoteEvent );
	virtual int HandleSDLEvent( SDL_Event event );
};
