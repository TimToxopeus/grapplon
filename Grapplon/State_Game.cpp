#include "State_Game.h"
#include "ResourceManager.h"
#include "WiimoteManager.h"
#include "ODEManager.h"
#include "PlayerObject.h"
#include "SoundManager.h"
#include "math.h"
#include "Texture.h"
#include "Sound.h"
#include "Universe.h"

#pragma warning(disable:4244)

CGameState::CGameState()
{
	for ( int i = 0; i<4; i++ )
		m_pPlayers[i] = NULL;
	m_pUniverse = NULL;

	m_bRunning = true;

	box.x = 20;
	box.y = 10;
	box.w = 256;
	box.h = 256;

	SetDepth( -10.0f );

//	CSound *pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/xpstart.wav", RT_SOUND);
//	pSound->Play();
//	CSoundManager::Instance()->LoadSound( "media/music/exit.ogg" );

	m_pSpace = (CTexture *)CResourceManager::Instance()->GetResource("media/images/starbg_HD.png", RT_TEXTURE);

	m_pPlayers[0] = new CPlayerObject(0);
	m_pPlayers[0]->SetPosition( Vector(500, 200, 0) );
	CWiimoteManager::Instance()->RegisterListener( m_pPlayers[0], 0 );

	m_pPlayers[1] = new CPlayerObject(1);
	m_pPlayers[1]->SetPosition( 200, 400 );
	CWiimoteManager::Instance()->RegisterListener( m_pPlayers[1], 1 );

	m_pPlayers[2] = new CPlayerObject(2);
	m_pPlayers[2]->SetPosition( 600, 500 );
	CWiimoteManager::Instance()->RegisterListener( m_pPlayers[2], 2 );

	m_pPlayers[3] = new CPlayerObject(3);
	m_pPlayers[3]->SetPosition( 300, 500 );
	CWiimoteManager::Instance()->RegisterListener( m_pPlayers[3], 3 );

	m_pPlayers[0]->SetDepth( 1.0f );
	m_pPlayers[1]->SetDepth( -1.0f );
	m_pPlayers[2]->SetDepth( 0.0f );
	m_pPlayers[3]->SetDepth( 2.0f );

	m_pPlayers[0]->SetMass( 10000.0f );
	m_pPlayers[1]->SetMass( 100.0f );
	m_pPlayers[2]->SetMass( 10000.0f );
	m_pPlayers[3]->SetMass( 100.0f );

	m_pUniverse = new CUniverse();
	m_pUniverse->Load( "media/scripts/alpha.txt" );

	m_pPlayers[3]->SetVelocity( Vector( 25.0f, 0.0f, 0.0f ) );
}

CGameState::~CGameState()
{
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
	fullscreen.x = fullscreen.y = 0;
	fullscreen.w = 1024;
	fullscreen.h = 768;
	RenderQuad( fullscreen, m_pSpace, 0 );
}

void CGameState::Update(float fTime)
{
}

bool CGameState::HandleWiimoteEvent( wiimote_t* pWiimoteEvent )
{
	if ( pWiimoteEvent->event == WIIUSE_EVENT )
	{
		struct nunchuk_t* nc = (nunchuk_t*)&pWiimoteEvent->exp.nunchuk;
		if ( nc->js.mag > 0.3f )
		{
			float angle = (nc->js.ang - 90.0f) * (3.14f / 180.0f);
			box.x += cos(angle) * (5.0f * nc->js.mag);
			box.y += sin(angle) * (5.0f * nc->js.mag);
			return true;
		}
	}
	return false;
}

int CGameState::HandleSDLEvent(SDL_Event event)
{
	if ( event.type == SDL_QUIT )
		m_bRunning = false;
	if ( event.type == SDL_KEYUP )
	{
		if ( event.key.keysym.sym == SDLK_k )
			m_pUniverse->SetUpOrbits();
	}
	return 0;
}
