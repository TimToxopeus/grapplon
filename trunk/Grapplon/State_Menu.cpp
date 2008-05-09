#include "State_Menu.h"

CMenuState::CMenuState( bool m_bSplash )
{
	m_bRunning = true;
	m_bQuit = false;
	m_bNext = false;

	m_fSplashAlpha = m_fAlpha = 0.0f;

	if ( m_bSplash )
	{
		state = SPLASH1;
		m_pSplash = new CAnimatedTexture("media/scripts/splash1.txt");
	}
	else
	{
		state = MENU2;
		m_pSplash = new CAnimatedTexture( "media/scripts/title.txt" );
		m_fSplashAlpha = m_fAlpha = 1.0f;
	}

	m_pBar = new CAnimatedTexture("media/scripts/bar.txt");
	m_pAB = new CAnimatedTexture("media/scripts/ab.txt");
	add = true;
}

CMenuState::~CMenuState()
{
	delete m_pSplash;
	delete m_pAB;
	delete m_pBar;
}

void CMenuState::Render()
{
	SDL_Rect target;
	if ( state == SPLASH1 || state == SPLASH2 || state == SPLASH3 )
	{
/*		target = m_pSplash->GetSize();
		target.x = -(target.w / 2);
		target.y = -(target.h / 2);*/
		target.x = -1024;
		target.y = -768;
		target.w = 2048;
		target.h = 1536;
	}
	else
	{
		target.x = -1024;
		target.y = -768;
		target.w = 2048;
		target.h = 1536;
	}
	RenderQuad( target, m_pSplash, 0, m_fSplashAlpha );

	if ( state == MENU1 || state == MENU2 )
	{
		target = m_pBar->GetSize();
		target.w *= 2;
		target.h *= 2;
		target.x = -1024;
		target.y = 32;
		if ( state == MENU1 && m_fAlpha >= 0.0f )
			RenderQuad( target, m_pBar, 0, m_fAlpha );

		if ( state == MENU2 )
		{
			RenderQuad( target, m_pBar, 0, 0.8f );
			target = m_pAB->GetSize();
			target.w *= (1.8f + 0.2f * (1.0f - m_fAlpha));
			target.h *= (1.8f + 0.2f * (1.0f - m_fAlpha));
			target.x = -(target.w / 2);
			target.y = 93 - (target.h / 2);
			RenderQuad( target, m_pAB, 0, 1.0f );
		}
	}
}

void CMenuState::Update(float fTime)
{
	if ( m_bNext && state < MENU2 )
	{
		NextState();
		m_bNext = false;
	}
	if ( state == SPLASH1 || state == SPLASH2 || state == SPLASH3 )
	{
		if ( !add )
			m_fSplashAlpha -= 0.33f * fTime;
		else
			m_fSplashAlpha += 0.33f * fTime;
		if ( m_fSplashAlpha > 1.0f )
		{
			m_fSplashAlpha = 1.0f;
			add = false;
		}
		if ( m_fSplashAlpha < 0.0f )
		{
			m_fSplashAlpha = 0.0f;
			add = true;

			NextState();
		}
	}
	else if ( state == MENU1 )
	{
		if ( !add )
			m_fAlpha -= 0.5f * fTime;
		else
			m_fAlpha += 0.5f * fTime;
		if ( m_fAlpha > 0.8f )
		{
			m_fAlpha = 0.8f;
			add = false;

			NextState();
		}
	}
	else if ( state == MENU2 )
	{
		if ( !add )
			m_fAlpha -= 1.0f * fTime;
		else
			m_fAlpha += 1.0f * fTime;
		if ( m_fAlpha > 1.0f )
		{
			m_fAlpha = 1.0f;
			add = false;
		}
		if ( m_fAlpha < 0.0f )
		{
			m_fAlpha = 0.0f;
			add = true;
		}
	}
}

bool CMenuState::HandleWiimoteEvent( wiimote_t* pWiimoteEvent )
{
	if ( pWiimoteEvent->event == WIIUSE_EVENT )
	{
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_A))
			m_bNext = true;
		if (IS_JUST_PRESSED(pWiimoteEvent, WIIMOTE_BUTTON_B))
			m_bNext = true;
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
		if ( state < MENU1 )
			NextState();

		if ( event.key.keysym.sym == SDLK_SPACE )
		{
			m_bRunning = false;
		}
	}
	if ( event.type == SDL_MOUSEBUTTONUP )
	{
		if ( state < MENU1 )
			NextState();
	}
	return 0;
}

void CMenuState::NextState()
{
	state++;

	if ( state == SPLASH2 )
	{
		CAnimatedTexture *pOld = m_pSplash;
		CAnimatedTexture *pNew = new CAnimatedTexture( "media/scripts/splash2.txt" );
		m_pSplash = pNew;
		delete pOld;
		m_fSplashAlpha = 0.0f;
		add = true;
	}
	if ( state == SPLASH3 )
	{
		CAnimatedTexture *pOld = m_pSplash;
		CAnimatedTexture *pNew = new CAnimatedTexture( "media/scripts/splash3.txt" );
		m_pSplash = pNew;
		delete pOld;
		m_fSplashAlpha = 0.0f;
		add = true;
	}
	if ( state == MENU1 )
	{
		CAnimatedTexture *pOld = m_pSplash;
		CAnimatedTexture *pNew = new CAnimatedTexture( "media/scripts/title.txt" );
		m_pSplash = pNew;
		delete pOld;
		m_fSplashAlpha = 1.0f;
		m_fAlpha = -1.0f;
		add = true;
	}
}
