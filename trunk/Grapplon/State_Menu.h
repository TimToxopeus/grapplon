#pragma once

#include "StateManager.h"
#include "AnimatedTexture.h"

#include <vector>

#define ABMENU 9
#define GAMEMENU 16
#define IR_AVG 20

enum StateStyle
{
	INSTANT = 0,
	FADE_IN,
	FADE_OUT,
	MOVE_UP,
	PULSE,
};

class StateChange
{
public:
	StateChange( int iState, int iSkipState, CAnimatedTexture *pImage, StateStyle eStyle, bool bIncState, int iStayRendered, float fStartAlpha, float fTime, int iStartX, int iStartY, int iGoalX, int iGoalY );

	int m_iState, m_iSkipState;
	CAnimatedTexture *m_pImage;
	StateStyle m_eStyle;
	bool m_bIncState;
	int m_iStayRendered;

	float m_fAlpha;
	float m_fTime, m_fTimeLeft;

	int m_iX, m_iY;
	int m_iStartX, m_iStartY;
	int m_iGoalX, m_iGoalY;
};

class CMenuState : public IStateManager
{
protected:
	CAnimatedTexture *m_pSplash1, *m_pSplash2, *m_pSplash3, *m_pTitle;
	CAnimatedTexture *m_pAB;

	CAnimatedTexture *m_pLogo, *m_pNintendo;

	CAnimatedTexture *m_pLogo2;
	CAnimatedTexture *m_pMenuBackground;
	CAnimatedTexture *m_pMenuSingleplayer;
	CAnimatedTexture *m_pMenuMultiplayer;
	CAnimatedTexture *m_pMenuHighscore;
	CAnimatedTexture *m_pMenuExit;
	CAnimatedTexture *m_pCursor;

	bool m_bNext;

	int cursorX, cursorY;
	int cursorXAvg[IR_AVG], cursorYAvg[IR_AVG];

	int state, skipstate;
	void NextState();

	std::vector<StateChange> m_vStates;

	void PushButton();

public:
	CMenuState( bool m_bSplash = true );
	virtual ~CMenuState();

	virtual void Render();
	virtual void Update( float fTime );
	virtual bool HandleWiimoteEvent( wiimote_t* pWiimoteEvent );
	virtual int HandleSDLEvent( SDL_Event event );
};
