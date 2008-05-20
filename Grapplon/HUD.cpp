#include "HUD.h"
#include "PlayerObject.h"
#include "Renderer.h"
#include "LogManager.h"
#include "AnimatedTexture.h"

CHUD::CHUD()
{
	for ( int i = 0; i<4; i++ )
		m_pPlayers[i] = NULL;
	SetDepth( 100.0f );
	m_eType = HUD;
	m_pNumbers = new CAnimatedTexture( "media/scripts/texture_numbers.txt" );

	m_pBorders[0] = new CAnimatedTexture( "media/scripts/texture_hud_red_border.txt" );
	m_pBorders[1] = new CAnimatedTexture( "media/scripts/texture_hud_green_border.txt" );
	m_pBorders[2] = new CAnimatedTexture( "media/scripts/texture_hud_blue_border.txt" );
	m_pBorders[3] = new CAnimatedTexture( "media/scripts/texture_hud_purple_border.txt" );

	m_pHealth[0] = new CAnimatedTexture( "media/scripts/texture_hud_red_bar.txt" );
	m_pHealth[1] = new CAnimatedTexture( "media/scripts/texture_hud_green_bar.txt" );
	m_pHealth[2] = new CAnimatedTexture( "media/scripts/texture_hud_blue_bar.txt" );
	m_pHealth[3] = new CAnimatedTexture( "media/scripts/texture_hud_purple_bar.txt" );
}

CHUD::~CHUD()
{
	delete m_pNumbers;

	for ( int i = 0; i<4; i++ )
	{
		delete m_pBorders[i];
		delete m_pHealth[i];
	}
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
		DrawHitpointBar( -1000, -744, c, 0, ((float)m_pPlayers[0]->GetHitpoints() / (float)m_pPlayers[0]->GetMaxHitpoints()) );
		DrawScoreBar( -1000, -700, c, m_pPlayers[0]->m_iScore );
	}
	if ( m_pPlayers[1] )
	{
		// Draw life
		SDL_Color c;
		c.r = c.g = c.b = 0; c.g = 255;
		DrawHitpointBar( 600, -744, c, 1, ((float)m_pPlayers[1]->GetHitpoints() / (float)m_pPlayers[1]->GetMaxHitpoints()) );
		DrawScoreBar( 1000, -700, c, m_pPlayers[1]->m_iScore, true );
	}
	if ( m_pPlayers[2] )
	{
		// Draw life
		SDL_Color c;
		c.r = 255; c.g = 0; c.b = 255;
		DrawHitpointBar( -1000, 704, c, 2, ((float)m_pPlayers[2]->GetHitpoints() / (float)m_pPlayers[2]->GetMaxHitpoints()) );
		DrawScoreBar( -1000, 680, c, m_pPlayers[2]->m_iScore );
	}
	if ( m_pPlayers[3] )
	{
		// Draw life
		SDL_Color c;
		c.r = 255; c.g = 255; c.b = 0;
		DrawHitpointBar( 600, 704, c, 3, ((float)m_pPlayers[3]->GetHitpoints() / (float)m_pPlayers[3]->GetMaxHitpoints()) );
		DrawScoreBar( 1000, 680, c, m_pPlayers[3]->m_iScore, true );
	}
}

void CHUD::DrawHitpointBar( int x, int y, SDL_Color c, int border, float healthRatio )
{
	SDL_Rect target = m_pBorders[border]->GetSize();
	target.x = x;
	target.y = y;
	RenderQuad( target, m_pBorders[border], 0, 1.0f );

	m_pHealth[border]->OverrideHeight( healthRatio );
	target.y = y + (target.h * (1.0f - healthRatio));
	target.h = target.h - (target.h * (1.0f - healthRatio));
	RenderQuad( target, m_pHealth[border], 0, 1.0f );
}

void CHUD::DrawScoreBar( int x, int y, SDL_Color c, int score, bool rtl )
{
	SDL_Rect target;
	target.w = target.h = 20;
	target.y = y;

	std::string szScore = itoa2(score);
	if ( rtl )
	{
		int l = (int)szScore.length();
		for ( int a = l - 1; a >= 0; a-- )
		{
			unsigned int v = (unsigned int)(szScore[(l - 1) - a] - 48);
			m_pNumbers->SetFrame( v );

			target.x = x - 24 * (a + 1);
			RenderQuad( target, m_pNumbers, 0, c, 1 );
		}
	}
	else
	{
		for ( unsigned int a = 0; a<szScore.length(); a++ )
		{
			unsigned int v = (unsigned int)(szScore[a] - 48);
			m_pNumbers->SetFrame( v );

			target.x = x + 24 * a;
			RenderQuad( target, m_pNumbers, 0, 1 );
		}
	}
}
