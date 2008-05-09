#pragma once

#include "StateManager.h"
#include "AnimatedTexture.h"

#define SPLASH1	1
#define SPLASH2 2
#define SPLASH3 3
#define MENU1 4
#define MENU2 5

class CMenuState : public IStateManager
{
protected:
	CAnimatedTexture *m_pSplash;
	CAnimatedTexture *m_pBar;
	CAnimatedTexture *m_pAB;
	float m_fSplashAlpha;
	bool add;
	bool m_bNext;

	int state;
	void NextState();

public:
	CMenuState( bool m_bSplash = true );
	virtual ~CMenuState();

	virtual void Render();
	virtual void Update( float fTime );
	virtual bool HandleWiimoteEvent( wiimote_t* pWiimoteEvent );
	virtual int HandleSDLEvent( SDL_Event event );
};
