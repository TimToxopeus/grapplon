#pragma once

#include "StateManager.h"
#include <sdl.h>

// Forward declaration
class CAnimatedTexture;
class CPlayerObject;
class CUniverse;
class CBackground;

class CGameState : public IStateManager
{
private:
	SDL_Rect box;
	CPlayerObject *m_pPlayers[4];
	CUniverse *m_pUniverse;

	CBackground *m_pBackground;
	void DrawHitpointBar( int x, int y, SDL_Color c, int width );

public:
	CGameState();
	virtual ~CGameState();

	bool Init( int iPlayers );
	virtual void Render();
	virtual void Update( float fTime );
	virtual bool HandleWiimoteEvent( wiimote_t* pWiimoteEvent );
	virtual int HandleSDLEvent( SDL_Event event );
};
