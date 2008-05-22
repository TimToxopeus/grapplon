#include "State_Menu.h"

StateChange::StateChange( int iState, int iSkipState, CAnimatedTexture *pImage, StateStyle eStyle, bool bIncState, int iStayRendered, float fStartAlpha, float fTime, int iStartX, int iStartY, int iGoalX, int iGoalY )
{
	m_iState = iState;
	m_iSkipState = iSkipState;
	m_pImage = pImage;
	m_eStyle = eStyle;
	m_bIncState = bIncState;
	m_iStayRendered = iStayRendered;
	m_fAlpha = fStartAlpha;
	m_fTime = m_fTimeLeft = fTime;
	m_iStartX = iStartX;
	m_iStartY = iStartY;
	m_iGoalX = iGoalX;
	m_iGoalY = iGoalY;
}

CMenuState::CMenuState( bool m_bSplash )
{
	m_bRunning = true;
	m_bQuit = false;
	m_bNext = false;

	if ( m_bSplash )
	{
		state = 0;
	}
	else
	{
		state = MENU;
	}

	m_pSplash1 = new CAnimatedTexture("media/scripts/texture_splash1.txt");
	m_pSplash2 = new CAnimatedTexture("media/scripts/texture_splash2.txt");
	m_pSplash3 = new CAnimatedTexture("media/scripts/texture_splash3.txt");
	m_pTitle = new CAnimatedTexture( "media/scripts/texture_title.txt" );

	m_pLogo = new CAnimatedTexture("media/scripts/texture_menu_logo.txt");
	m_pNintendo = new CAnimatedTexture("media/scripts/texture_menu_nintendo.txt");
	m_pAB = new CAnimatedTexture("media/scripts/texture_ab.txt");

	m_pMenuBackground = new CAnimatedTexture("media/scripts/texture_menu_background.txt");
	m_pMenuSingleplayer = new CAnimatedTexture("media/scripts/texture_menu_singleplayer.txt");
	m_pMenuMultiplayer = new CAnimatedTexture("media/scripts/texture_menu_multiplayer.txt");
	m_pMenuHighscore = new CAnimatedTexture("media/scripts/texture_menu_highscore.txt");

	m_vStates.push_back( StateChange( 0, 2, m_pSplash1, FADE_IN, true, 0, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 1, 2, m_pSplash1, FADE_OUT, true, 1, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 2, 4, m_pSplash2, FADE_IN, true, 2, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 3, 4, m_pSplash2, FADE_OUT, true, 3, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 4, 6, m_pSplash3, FADE_IN, true, 4, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 5, 6, m_pSplash3, FADE_OUT, true, 5, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 6, 9, m_pTitle, FADE_IN, true, 10, 1.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 7, 9, m_pLogo, FADE_IN, true, 10, 1.0f, 2.0f, -746, -286, -746, -286 ) );
	m_vStates.push_back( StateChange( 8, 9, m_pNintendo, FADE_IN, true, 10, 1.0f, 2.0f, 664, -768, 664, -768 ) );
	m_vStates.push_back( StateChange( 9, 9, m_pAB, PULSE, false, 10, 0.0f, 1.0f, -450, 67, -450, 67 ) );

	for ( int i = 0; i<IR_AVG; i++ )
	{
		cursorXAvg[i] = 0;
		cursorYAvg[i] = 0;
	}
}

CMenuState::~CMenuState()
{
	delete m_pSplash1;
	delete m_pSplash2;
	delete m_pSplash3;
	delete m_pTitle;

	delete m_pLogo;
	delete m_pNintendo;
	delete m_pAB;

	delete m_pMenuBackground;
	delete m_pMenuSingleplayer;
	delete m_pMenuMultiplayer;
	delete m_pMenuHighscore;
}

void CMenuState::Render()
{
	SDL_Rect target;
	for ( unsigned int a = 0; a<m_vStates.size(); a++ )
	{
		if ( m_vStates[a].m_iState == state || (m_vStates[a].m_iState < state && m_vStates[a].m_iStayRendered >= state) )
		{
			target = m_vStates[a].m_pImage->GetSize();
			target.w += target.w;
			target.h += target.h;
			target.x = m_vStates[a].m_iX;
			target.y = m_vStates[a].m_iY;

			RenderQuad( target, m_vStates[a].m_pImage, 0, m_vStates[a].m_fAlpha );
		}
	}
}

void CMenuState::Update(float fTime)
{
	if ( m_bNext && state < MENU )
	{
		NextState();
		m_bNext = false;
	}
	for ( unsigned int a = 0; a<m_vStates.size(); a++ )
	{
		if ( m_vStates[a].m_iState == state )
		{
			m_vStates[a].m_fTimeLeft -= fTime;
			float ratio = m_vStates[a].m_fTimeLeft / m_vStates[a].m_fTime;

			m_vStates[a].m_iX = m_vStates[a].m_iStartX + (int)((m_vStates[a].m_iGoalX - m_vStates[a].m_iStartX) * ratio);
			m_vStates[a].m_iY = m_vStates[a].m_iStartY + (int)((m_vStates[a].m_iGoalY - m_vStates[a].m_iStartY) * ratio);

			if ( m_vStates[a].m_eStyle == FADE_IN )
				m_vStates[a].m_fAlpha = 1.0f - ratio;
			if ( m_vStates[a].m_eStyle == FADE_OUT )
				m_vStates[a].m_fAlpha = ratio;
			if ( m_vStates[a].m_eStyle == PULSE )
			{
				if ( ratio < 0.5f )
					m_vStates[a].m_fAlpha = 0.5f + 0.5f * (ratio * 2);
				else
					m_vStates[a].m_fAlpha = 0.5f + 0.5f * (1.0f - ((ratio - 0.5f) * 2));
			}

			if ( m_vStates[a].m_fTimeLeft <= 0.0f )
			{
				if ( m_vStates[a].m_bIncState )
					state++;

				if ( m_vStates[a].m_eStyle == PULSE )
					m_vStates[a].m_fTimeLeft = m_vStates[a].m_fTime;
			}
		}
	}
}

bool CMenuState::HandleWiimoteEvent( wiimote_t* pWiimoteEvent )
{
	if ( pWiimoteEvent->event == WIIUSE_EVENT )
	{
		if ( state <= MENU )
		{
			if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A))
				m_bNext = true;
			if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B))
				m_bNext = true;
		}
/*		else if ( state == MENU2 )
		{
			if ( IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A) && IS_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B) )
				state = MENU3;
			if ( IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B) && IS_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A) )
				state = MENU3;
		}
		else if ( state == MENU2 )
		{
			if ( IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A) )
			{
				m_bRunning = false;
			}
		}*/
		if ( WIIUSE_USING_IR( pWiimoteEvent ) )
		{
			// Check if the new value doesn't crazily exceed old average
			int deltaX = abs(cursorX - pWiimoteEvent->ir.x);
			int deltaY = abs(cursorY - pWiimoteEvent->ir.y);

			if ( deltaX < 512 && deltaY < 384 )
			{
				for ( int i = 0; i<(IR_AVG - 1); i++ )
				{
					cursorXAvg[i] = cursorXAvg[i + 1];
					cursorYAvg[i] = cursorYAvg[i + 1];
				}

				cursorXAvg[IR_AVG - 1] = pWiimoteEvent->ir.x;
				cursorYAvg[IR_AVG - 1] = pWiimoteEvent->ir.y;

				cursorX = 0; cursorY = 0;
				for ( int i = 0; i < IR_AVG; i++ )
				{
					cursorX += cursorXAvg[i];
					cursorY += cursorYAvg[i];
				}

				cursorX /= IR_AVG;
				cursorY /= IR_AVG;
			}
		}
	}
	return false;
}

int CMenuState::HandleSDLEvent(SDL_Event event)
{
	if ( event.type == SDL_QUIT )
	{
		m_bRunning = false;
		m_bQuit = true;
	}
	if ( event.type == SDL_KEYUP )
	{
		if ( state < MENU )
			NextState();

		if ( event.key.keysym.sym == SDLK_SPACE )
		{
			m_bRunning = false;
		}
		if ( event.key.keysym.sym == SDLK_p )
		{
			state = MENU;
		}
	}
	if ( event.type == SDL_MOUSEBUTTONUP )
	{
		if ( state < MENU )
			NextState();
	}
	return 0;
}

void CMenuState::NextState()
{
	state = m_vStates[state].m_iSkipState;

	for ( unsigned i = 0; i<m_vStates.size(); i++ )
	{
		if ( m_vStates[i].m_iState < state && (m_vStates[i].m_iStayRendered >= state) )
		{
			if ( m_vStates[i].m_eStyle == FADE_IN )
				m_vStates[i].m_fAlpha = 1.0f;
			if ( m_vStates[i].m_eStyle == FADE_OUT )
				m_vStates[i].m_fAlpha = 0.0f;

			m_vStates[i].m_iX = m_vStates[i].m_iGoalX;
			m_vStates[i].m_iY = m_vStates[i].m_iGoalY;
		}
	}
/*
	if ( state == SPLASH2 )
	{
		CAnimatedTexture *pOld = m_pSplash;
		CAnimatedTexture *pNew = new CAnimatedTexture( "media/scripts/texture_splash2.txt" );
		m_pSplash = pNew;
		delete pOld;
		m_fSplashAlpha = 0.0f;
		add = true;
	}
	if ( state == SPLASH3 )
	{
		CAnimatedTexture *pOld = m_pSplash;
		CAnimatedTexture *pNew = new CAnimatedTexture( "media/scripts/texture_splash3.txt" );
		m_pSplash = pNew;
		delete pOld;
		m_fSplashAlpha = 0.0f;
		add = true;
	}
	if ( state == MENU1 )
	{
		CAnimatedTexture *pOld = m_pSplash;
		CAnimatedTexture *pNew = new CAnimatedTexture( "media/scripts/texture_title.txt" );
		m_pSplash = pNew;
		delete pOld;
		m_fSplashAlpha = 1.0f;
		m_fAlpha = -1.0f;
		add = true;
	}*/
}
