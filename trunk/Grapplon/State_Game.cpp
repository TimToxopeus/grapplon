#include "State_Game.h"
#include "ResourceManager.h"
#include "WiimoteManager.h"
#include "ODEManager.h"
#include "PlayerObject.h"
#include "SoundManager.h"
#include "math.h"
#include "AnimatedTexture.h"
#include "Sound.h"
#include "Universe.h"
#include "Renderer.h"

CGameState::CGameState()
{
	for ( int i = 0; i<4; i++ )
		m_pPlayers[i] = NULL;
	m_pUniverse = NULL;

	m_bRunning = true;
	m_bQuit = false;

	box.x = 20;
	box.y = 10;
	box.w = 256;
	box.h = 256;

	SetDepth( -10.0f );
}

bool CGameState::Init( int iPlayers )
{

//	CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/xpstart.wav", RT_SOUND);
//	pSound->Play();
//	CSoundManager::Instance()->LoadSound( "media/music/exit.ogg" );

	m_pSpace = new CAnimatedTexture("media/scripts/starbg_HD.txt");

	if ( iPlayers > 0 )
	{
		m_pPlayers[0] = new CPlayerObject(0);
		m_pPlayers[0]->SetPosition( Vector(500, 200, 0) );
		CWiimoteManager::Instance()->RegisterListener( m_pPlayers[0], 0 );
	}

	if ( iPlayers > 1 )
	{
		m_pPlayers[1] = new CPlayerObject(1);
		m_pPlayers[1]->SetPosition( -200, 400 );
		CWiimoteManager::Instance()->RegisterListener( m_pPlayers[1], 1 );
	}

	if ( iPlayers > 2 )
	{
		m_pPlayers[2] = new CPlayerObject(2);
		m_pPlayers[2]->SetPosition( 600, 500 );
		CWiimoteManager::Instance()->RegisterListener( m_pPlayers[2], 2 );
	}

	if ( iPlayers > 3 )
	{
		m_pPlayers[3] = new CPlayerObject(3);
		m_pPlayers[3]->SetPosition( 300, 500 );
		CWiimoteManager::Instance()->RegisterListener( m_pPlayers[3], 3 );
	}

//	m_pPlayers[0]->SetDepth( 1.0f );

//	m_pPlayers[1]->SetDepth( -1.0f );
//	m_pPlayers[2]->SetDepth( 0.0f );
//	m_pPlayers[3]->SetDepth( 2.0f );

/*	m_pPlayers[0]->SetMass( 10000.0f );
	m_pPlayers[1]->SetMass( 100.0f );
	m_pPlayers[2]->SetMass( 10000.0f );
	m_pPlayers[3]->SetMass( 100.0f );*/

	m_pUniverse = new CUniverse();
	m_pUniverse->Load( "media/scripts/alpha.txt" );

	//m_pPlayers[0]->SetVelocity( Vector( 0.0f, 2500.0f, 0.0f ) );
	return true;
}

CGameState::~CGameState()
{
	delete m_pSpace;

	for ( int i = 0; i<4; i++ )
	{
		if ( m_pPlayers[i] )
		{
			delete m_pPlayers[i];
			m_pPlayers[i] = NULL;
		}
	}
	if ( m_pUniverse )
	{
		delete m_pUniverse;
		m_pUniverse = NULL;
	}
}

void CGameState::Render()
{
	SDL_Rect fullscreen;
//	fullscreen.w = 1024;
//	fullscreen.h = 768;
	fullscreen.w = 2048;
	fullscreen.h = 1536;
	fullscreen.x = -1024;
	fullscreen.y = -768;

	CRenderer::Instance()->SetCamera( Vector( 0, 0, 0 ), 2.0f );

	RenderQuad( fullscreen, m_pSpace, 0 );

	Vector playerCenter;
	float c = 0;
	for ( int i = 0; i<4; i++ )
	{
		if ( m_pPlayers[i] )
		{
			playerCenter += m_pPlayers[i]->GetPosition();
			c += 1.0f;
		}
	}
	playerCenter /= c;

	// Furthest distance
	float distance = 0.0f;
	for ( int i = 0; i<4; i++ )
	{
		if ( m_pPlayers[i] )
		{
			float d = (playerCenter - m_pPlayers[i]->GetPosition()).Length();
			if ( d > distance )
				distance = d;
		}
	}
	float zoom = distance / 300.0f;
	if ( zoom < 2.0f )
		zoom = 2.0f;
	if ( zoom > 4.0f )
		zoom = 4.0f;

	if ( m_pPlayers[0] )
	{
		// Draw life
		SDL_Color c;
		c.r = c.g = c.b = 0; c.r = 255;
		int width = (int)(392.0f * ((float)m_pPlayers[0]->GetHitpoints() / (float)m_pPlayers[0]->GetMaxHitpoints()));
		DrawHitpointBar( -1000, -744, c, width );
	}
	if ( m_pPlayers[1] )
	{
		// Draw life
		SDL_Color c;
		c.r = c.g = c.b = 0; c.g = 255;
		int width = (int)(392.0f * ((float)m_pPlayers[1]->GetHitpoints() / (float)m_pPlayers[1]->GetMaxHitpoints()));
		DrawHitpointBar( 600, -744, c, width );
	}
	if ( m_pPlayers[2] )
	{
		// Draw life
		SDL_Color c;
		c.r = 255; c.g = 0; c.b = 255;
		int width = (int)(392.0f * ((float)m_pPlayers[1]->GetHitpoints() / (float)m_pPlayers[1]->GetMaxHitpoints()));
		DrawHitpointBar( 1000, 704, c, width );
	}
	if ( m_pPlayers[3] )
	{
		// Draw life
		SDL_Color c;
		c.r = 255; c.g = 255; c.b = 0;
		int width = (int)(392.0f * ((float)m_pPlayers[1]->GetHitpoints() / (float)m_pPlayers[1]->GetMaxHitpoints()));
		DrawHitpointBar( 600, 704, c, width );
	}

	FILE *pFile = fopen( "maxzoom.txt", "rt" );
	if ( !pFile )
	{
		CRenderer::Instance()->SetCamera( playerCenter, zoom );
	}
	else
	{
		CRenderer::Instance()->SetCamera( playerCenter, 4.0f );
		fclose(pFile);
	}
/*
	fullscreen.x = -1024;
	RenderQuad( fullscreen, m_pSpace, 0 );
	fullscreen.y = -768;
	RenderQuad( fullscreen, m_pSpace, 0 );
	fullscreen.x = 0;
	RenderQuad( fullscreen, m_pSpace, 0 );
*/
}

void CGameState::Update(float fTime)
{
	m_pSpace->UpdateFrame( fTime );
}

bool CGameState::HandleWiimoteEvent( wiimote_t* pWiimoteEvent )
{
	if ( pWiimoteEvent->event == WIIUSE_EVENT )
	{
		struct nunchuk_t* nc = (nunchuk_t*)&pWiimoteEvent->exp.nunchuk;
		if ( nc->js.mag > 0.3f )
		{
			float angle = (nc->js.ang - 90.0f) * (3.14f / 180.0f);
			box.x += (int)(cos(angle) * (5.0f * nc->js.mag));
			box.y += (int)(sin(angle) * (5.0f * nc->js.mag));
			return true;
		}
	}
	return false;
}

int CGameState::HandleSDLEvent(SDL_Event event)
{
	if ( event.type == SDL_QUIT )
	{
		m_bRunning = false;
		m_bQuit = true;
	}
	if ( event.type == SDL_KEYUP )
	{
		if ( event.key.keysym.sym == SDLK_k )
			m_pUniverse->SetUpOrbits();

		if ( event.key.keysym.sym == SDLK_SPACE )
		{
			m_bRunning = false;
		}
	}
	return 0;
}

void CGameState::DrawHitpointBar( int x, int y, SDL_Color c, int width )
{
	SDL_Rect target;
	target.w = 400;
	target.h = 40;
	target.x = x;
	target.y = y;
	RenderQuad( target, NULL, 0, 1.0f );

	SDL_Color colour;
	colour.r = colour.g = colour.b = 0;

	target.w = 396;
	target.h = 36;
	target.x = x + 2;
	target.y = y + 2;
	RenderQuad( target, NULL, 0, colour, 1.0f );

	target.w = width;
	target.h = 32;
	target.x = x + 4;
	target.y = y + 4;
	RenderQuad( target, NULL, 0, c, 1.0f );
}
