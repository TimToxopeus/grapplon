#pragma once

#include "StateManager.h"
#include "AnimatedTexture.h"

#define SPLASH1	1
#define SPLASH2 2
#define SPLASH3 3
#define MENU1 4
#define MENU2 5
#define MENU3 6

#define IR_AVG 20

class CMenuState : public IStateManager
{
protected:
	CAnimatedTexture *m_pSplash;
	CAnimatedTexture *m_pBar;
	CAnimatedTexture *m_pAB;

	CAnimatedTexture *m_pMenuBackground;
	CAnimatedTexture *m_pMenuSingleplayer;
	CAnimatedTexture *m_pMenuMultiplayer;
	CAnimatedTexture *m_pMenuHighscore;

	float m_fSplashAlpha;
	bool add;
	bool m_bNext;

	int cursorX, cursorY;
	int cursorXAvg[IR_AVG], cursorYAvg[IR_AVG];

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
