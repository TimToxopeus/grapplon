#pragma once

#include "ActiveObject.h"
class CPlayerObject;

class CHUD : public IActiveObject
{
private:
	CPlayerObject *m_pPlayers[4];
	void DrawHitpointBar( int x, int y, SDL_Color c, int width );

public:
	CHUD();
	~CHUD();

	void SetPlayers( CPlayerObject *p1, CPlayerObject *p2, CPlayerObject *p3, CPlayerObject *p4 );

	void Update( float fTime );
	void Render();
};
