#include "Background.h"
#include "Renderer.h"
#include "AnimatedTexture.h"

CBackground::CBackground()
{
	SetDepth( -1000.0f );
	m_pSpace = new CAnimatedTexture("media/scripts/texture_bkg_space.txt");
	m_pNebula = new CAnimatedTexture("media/scripts/texture_bkg_movinglayer.txt");
	m_fNebulaOffset = -1024.0f;
}

CBackground::~CBackground()
{
	delete m_pSpace;
	delete m_pNebula;
}

void CBackground::Update( float fTime )
{
	m_pSpace->UpdateFrame( fTime );
	m_pNebula->UpdateFrame( fTime );
	m_fNebulaOffset += 75.0f * fTime;
	if ( m_fNebulaOffset >= 4096 )
		m_fNebulaOffset -= 4096;
}

void CBackground::Render()
{
	SDL_Rect fullscreen;
	fullscreen.w = 2048;
	fullscreen.h = 1536;
	fullscreen.x = -1024;
	fullscreen.y = -768;

	CRenderer::Instance()->SetCamera( Vector( 0, 0, 0 ), 2.0f );

	RenderQuad( fullscreen, m_pSpace, 0 );

	fullscreen.w = 4096;
	fullscreen.x = -1024 + (int)m_fNebulaOffset;
	RenderQuad( fullscreen, m_pNebula, 0, 0.6f );
	fullscreen.x = -(1024 + 4096) + (int)m_fNebulaOffset;
	RenderQuad( fullscreen, m_pNebula, 0, 0.6f );
}
