#include "Core.h"
#include "LogManager.h"

#include "Renderer.h"
#include "ResourceManager.h"
#include "State_Game.h"
#include "WiimoteManager.h"
#include "ODEManager.h"
#include "SoundManager.h"

CCore *CCore::m_pInstance = NULL;

CCore::CCore()
{
	m_pRenderer = NULL;
	m_pResourceManager = NULL;
	m_pActiveState = NULL;
	m_pWiimoteManager = NULL;
	m_pODEManager = NULL;
	m_pSoundManager = NULL;
}

CCore::~CCore()
{
}

bool CCore::SystemsInit()
{
	CLogManager::Instance()->LogMessage("Initializing engine.");

	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return false;

	// Initialize renderer
	m_pRenderer = CRenderer::Instance();
	if ( !m_pRenderer->Init() )
		return false;

	// Initialize resource manager
	m_pResourceManager = CResourceManager::Instance();

	// Initialize wiimote manager
	m_pWiimoteManager = CWiimoteManager::Instance();

	// Initialize ODE
	m_pODEManager = CODEManager::Instance();

	// Initialize sound
	m_pSoundManager = CSoundManager::Instance();
	if ( !m_pSoundManager->Init() )
		return false;

	// Initialize active state
	m_pActiveState = new CGameState();
	m_pWiimoteManager->RegisterListener( m_pActiveState, -1 );

	// All systems go!
	CLogManager::Instance()->LogMessage("Initializion succesful.");
	return true;
}

void CCore::SystemsDestroy()
{
	CLogManager::Instance()->LogMessage("Terminating engine.");

	// Terminate ODE Manager
	if ( m_pODEManager )
	{
		CODEManager::Destroy();
		m_pODEManager = NULL;
	}

	// Terminate state
	if ( m_pActiveState )
	{
		delete m_pActiveState;
		m_pActiveState = NULL;
	}

	// Terminate Wiimote manager
	if ( m_pWiimoteManager )
	{
		CWiimoteManager::Destroy();
		m_pWiimoteManager = NULL;
	}

	// Terminate resource manager
	if ( m_pResourceManager )
	{
		CResourceManager::Destroy();
		m_pResourceManager = NULL;
	}

	// Terminate sound manager
	if ( m_pSoundManager )
	{
		m_pSoundManager->Shutdown();
		CSoundManager::Destroy();
		m_pSoundManager = NULL;
	}

	// Terminate renderer
	if ( m_pRenderer )
	{
		m_pRenderer->Shutdown();
		CRenderer::Destroy();
		m_pRenderer = NULL;
	}

	CLogManager::Instance()->LogMessage("Termination complete.");
}

void CCore::Run()
{
	Uint32 time, lastUpdate;
	time = lastUpdate = SDL_GetTicks();
	SDL_Event event;

	static float stime = 0;
	static int frames = 0;

	// Start handling Wiimote events
	CLogManager::Instance()->LogMessage("Starting Wiimote thread.");
	m_pWiimoteManager->StartEventThread();

	// Game loop goes here
	CLogManager::Instance()->LogMessage("Starting game loop.");
	while ( IsRunning() )
	{
		// Handle SDL events
		while ( SDL_PollEvent( &event ) )
		{
			m_pActiveState->HandleSDLEvent( event );
		}

		// Do update
		time = SDL_GetTicks();
		float timeSinceLastUpdate = (float)(time - lastUpdate) / 1000.0f;
		m_pSoundManager->Update( timeSinceLastUpdate );
		m_pODEManager->Update( timeSinceLastUpdate );
		m_pRenderer->Update( timeSinceLastUpdate );
		lastUpdate = time;

		stime += timeSinceLastUpdate;
		frames++;
		if ( stime > 1.0f ) // 1 second has passed
		{
			CLogManager::Instance()->LogMessage("FPS: " + itoa2(frames));
			frames = 0;
			stime = 0.0f;
		}

		// Handle rendering
		m_pRenderer->Render();
	}

	// Stop handling Wiimote events
	CLogManager::Instance()->LogMessage("Stopping Wiimote thread.");
	m_pWiimoteManager->StopEventThread();
}

bool CCore::IsRunning()
{
	return m_pActiveState->IsRunning();
}
