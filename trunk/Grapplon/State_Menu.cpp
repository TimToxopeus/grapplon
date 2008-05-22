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
		skipstate = 9;
	}
	else
	{
		state = MENU;
		skipstate = 16;
	}

	m_pSplash1 = new CAnimatedTexture("media/scripts/texture_splash1.txt");
	m_pSplash2 = new CAnimatedTexture("media/scripts/texture_splash2.txt");
	m_pSplash3 = new CAnimatedTexture("media/scripts/texture_splash3.txt");
	m_pTitle = new CAnimatedTexture( "media/scripts/texture_title.txt" );

	m_pLogo = new CAnimatedTexture("media/scripts/texture_menu_logo.txt");
	m_pNintendo = new CAnimatedTexture("media/scripts/texture_menu_nintendo.txt");
	m_pAB = new CAnimatedTexture("media/scripts/texture_ab.txt");

	m_pLogo2 = new CAnimatedTexture("media/scripts/texture_menu_logo_small.txt");
	m_pMenuBackground = new CAnimatedTexture("media/scripts/texture_menu_background.txt");
	m_pMenuSingleplayer = new CAnimatedTexture("media/scripts/texture_menu_singleplayer.txt");
	m_pMenuMultiplayer = new CAnimatedTexture("media/scripts/texture_menu_multiplayer.txt");
	m_pMenuHighscore = new CAnimatedTexture("media/scripts/texture_menu_highscore.txt");
	m_pMenuExit = new CAnimatedTexture("media/scripts/texture_menu_exit.txt");

	m_vStates.push_back( StateChange( 0, 2, m_pSplash1, FADE_IN, true, 0, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 1, 2, m_pSplash1, FADE_OUT, true, 1, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 2, 4, m_pSplash2, FADE_IN, true, 2, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 3, 4, m_pSplash2, FADE_OUT, true, 3, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 4, 6, m_pSplash3, FADE_IN, true, 4, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );
	m_vStates.push_back( StateChange( 5, 6, m_pSplash3, FADE_OUT, true, 5, 0.0f, 2.0f, -1024, -768, -1024, -768 ) );

	m_vStates.push_back( StateChange( 6, 9, m_pTitle, FADE_IN, true, 20, 1.0f, 2.0f, -1024, -768, -1024, -768 ) );

	m_vStates.push_back( StateChange( 7, 9, m_pLogo, FADE_IN, true, 9, 1.0f, 2.0f, -746, -286, -746, -286 ) );
	m_vStates.push_back( StateChange( 8, 9, m_pNintendo, FADE_IN, true, 20, 1.0f, 2.0f, 664, -768, 664, -768 ) );
	m_vStates.push_back( StateChange( 9, 9, m_pAB, PULSE, false, 9, 0.0f, 1.0f, -450, 67, -450, 67 ) );

	m_vStates.push_back( StateChange( 10, 10, m_pLogo, FADE_OUT, true, 10, 1.0f, 1.0f, -746, -286, -746, -286 ) );
	m_vStates.push_back( StateChange( 10, 10, m_pAB, FADE_OUT, false, 10, 1.0f, 1.0f, -450, 67, -450, 67 ) );

	m_vStates.push_back( StateChange( 11, 16, m_pLogo2, MOVE_UP, true, 15, 1.0f, 1.4f, -516, 1000, -516, -366 ) );
	m_vStates.push_back( StateChange( 12, 16, m_pMenuSingleplayer, MOVE_UP, true, 15, 0.5f, 0.7f, -340, 1000, -340, -150 ) );
	m_vStates.push_back( StateChange( 13, 16, m_pMenuMultiplayer, MOVE_UP, true, 15, 0.5f, 0.7f, -340, 1000, -340, -30 ) );
	m_vStates.push_back( StateChange( 14, 16, m_pMenuHighscore, MOVE_UP, true, 15, 0.5f, 0.7f, -340, 1000, -340, 90 ) );
	m_vStates.push_back( StateChange( 15, 16, m_pMenuExit, MOVE_UP, true, 15, 0.5f, 0.7f, -340, 1000, -340, 210 ) );

	m_vStates.push_back( StateChange( 16, 16, m_pLogo2, INSTANT, false, 16, 1.0f, 0.0f, -516, -366, -516, -366 ) );
	m_vStates.push_back( StateChange( 16, 16, m_pMenuSingleplayer, INSTANT, false, 16, 0.5f, 0.0f, -340, -150, -340, -150 ) );
	m_vStates.push_back( StateChange( 16, 16, m_pMenuMultiplayer, INSTANT, false, 16, 0.5f, 0.0f, -340, -30, -340, -30 ) );
	m_vStates.push_back( StateChange( 16, 16, m_pMenuHighscore, INSTANT, false, 16, 0.5f, 0.0f, -340, 90, -340, 90 ) );
	m_vStates.push_back( StateChange( 16, 16, m_pMenuExit, INSTANT, false, 16, 0.5f, 0.0f, -340, 210, -340, 210 ) );

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

	delete m_pLogo2;
	delete m_pMenuBackground;
	delete m_pMenuSingleplayer;
	delete m_pMenuMultiplayer;
	delete m_pMenuHighscore;
	delete m_pMenuExit;
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
			skipstate = m_vStates[a].m_iSkipState;

			m_vStates[a].m_fTimeLeft -= fTime;
			float ratio = m_vStates[a].m_fTimeLeft / m_vStates[a].m_fTime;

			float ratioSquared = ratio * ratio;
			int dX = m_vStates[a].m_iStartX - m_vStates[a].m_iGoalX;
			int dY = m_vStates[a].m_iStartY - m_vStates[a].m_iGoalY;
			m_vStates[a].m_iX = m_vStates[a].m_iGoalX + (int)(dX * ratioSquared);
			m_vStates[a].m_iY = m_vStates[a].m_iGoalY + (int)(dY * ratioSquared);

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
		if ( event.key.keysym.sym == SDLK_SPACE )
		{
			m_bRunning = false;
		}
		else if ( event.key.keysym.sym == SDLK_p )
		{
			state = MENU;
		}
		else
			NextState();
	}
	if ( event.type == SDL_MOUSEBUTTONUP )
	{
		NextState();
	}
	return 0;
}

void CMenuState::NextState()
{
	state = skipstate;

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
}
