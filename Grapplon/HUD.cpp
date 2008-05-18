#include "HUD.h"
#include "PlayerObject.h"
#include "Renderer.h"

CHUD::CHUD()
{
	for ( int i = 0; i<4; i++ )
		m_pPlayers[i] = NULL;
	SetDepth( 100.0f );
	m_eType = HUD;
}

CHUD::~CHUD()
{
}

void CHUD::SetPlayers( CPlayerObject *p1, CPlayerObject *p2, CPlayerObject *p3, CPlayerObject *p4 )
{
	m_pPlayers[0] = p1;
	m_pPlayers[1] = p2;
	m_pPlayers[2] = p3;
	m_pPlayers[3] = p4;
}

void CHUD::Update( float fTime )
{
}

void CHUD::Render()
{
	CRenderer::Instance()->SetCamera( Vector( 0, 0, 0 ), 2.0f );
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
		int width = (int)(392.0f * ((float)m_pPlayers[2]->GetHitpoints() / (float)m_pPlayers[2]->GetMaxHitpoints()));
		DrawHitpointBar( 1000, 704, c, width );
	}
	if ( m_pPlayers[3] )
	{
		// Draw life
		SDL_Color c;
		c.r = 255; c.g = 255; c.b = 0;
		int width = (int)(392.0f * ((float)m_pPlayers[3]->GetHitpoints() / (float)m_pPlayers[3]->GetMaxHitpoints()));
		DrawHitpointBar( 600, 704, c, width );
	}
}

void CHUD::DrawHitpointBar( int x, int y, SDL_Color c, int width )
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

