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

	m_pBorders = new CAnimatedTexture( "media/scripts/texture_hud_border.txt" );
	m_pHealth = new CAnimatedTexture( "media/scripts/texture_hud_bar.txt" );
	m_pNumbers = new CAnimatedTexture( "media/scripts/texture_numbers.txt" );
}

CHUD::~CHUD()
{
	delete m_pNumbers;

	delete m_pBorders;
	delete m_pHealth;
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
		DrawScoreBar( -1000, -700, c, m_pPlayers[0]->m_iScore, true );
	}
	if ( m_pPlayers[1] )
	{
		// Draw life
		SDL_Color c;
		c.r = c.g = c.b = 0; c.g = 255;
		DrawHitpointBar( 680, -744, c, 1, ((float)m_pPlayers[1]->GetHitpoints() / (float)m_pPlayers[1]->GetMaxHitpoints()) );
		DrawScoreBar( 1000, -700, c, m_pPlayers[1]->m_iScore );
	}
	if ( m_pPlayers[2] )
	{
		// Draw life
		SDL_Color c;
		c.r = 255; c.g = 0; c.b = 255;
		DrawHitpointBar( -1000, 580, c, 2, ((float)m_pPlayers[2]->GetHitpoints() / (float)m_pPlayers[2]->GetMaxHitpoints()) );
		DrawScoreBar( -1000, 680, c, m_pPlayers[2]->m_iScore, true );
	}
	if ( m_pPlayers[3] )
	{
		// Draw life
		SDL_Color c;
		c.r = 255; c.g = 255; c.b = 0;
		DrawHitpointBar( 680, 580, c, 3, ((float)m_pPlayers[3]->GetHitpoints() / (float)m_pPlayers[3]->GetMaxHitpoints()) );
		DrawScoreBar( 1000, 680, c, m_pPlayers[3]->m_iScore );
	}
}

void CHUD::DrawHitpointBar( int x, int y, SDL_Color c, int border, float healthRatio )
{
	m_pBorders->SetAnimation( border );
	m_pHealth->SetAnimation( border );

	SDL_Rect target = m_pBorders->GetSize();
	target.x = x;
	target.y = y;
	RenderQuad( target, m_pBorders, 0, 1.0f );

	m_pHealth->OverrideHeight( healthRatio );
	target.y = y + (int)(target.h * (1.0f - healthRatio));
	target.h = target.h - (int)(target.h * (1.0f - healthRatio));
	RenderQuad( target, m_pHealth, 0, 1.0f );
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
