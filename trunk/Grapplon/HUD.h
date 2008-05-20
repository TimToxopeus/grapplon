#pragma once

#include "ActiveObject.h"
class CPlayerObject;
class CAnimatedTexture;

class CHUD : public IActiveObject
{
private:
	CPlayerObject *m_pPlayers[4];
	void DrawHitpointBar( int x, int y, int player, float healthRatio );
	void DrawScoreBar( int x, int y, int player, int score, bool rtl = false );
	CAnimatedTexture *m_pNumbers;

	CAnimatedTexture *m_pBorders;
	CAnimatedTexture *m_pHealth;

public:
	CHUD();
	~CHUD();

	void SetPlayers( CPlayerObject *p1, CPlayerObject *p2, CPlayerObject *p3, CPlayerObject *p4 );

	void Update( float fTime );
	void Render();
};
