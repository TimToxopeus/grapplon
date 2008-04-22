#pragma once

#include "BaseMovableObject.h"
#include "WiimoteListener.h"

class CPlayerObject : public CBaseMovableObject, public IWiimoteListener
{
private:
	float y,p,r;
	CTexture *m_pRadius;
	int m_iPlayer;

	float timeSinceNoInput;

public:
	CPlayerObject( int iPlayer );
	virtual ~CPlayerObject();

	virtual bool HandleWiimoteEvent( wiimote_t* pWiimoteEvent );
	virtual void Update( float fTime );
	virtual void Render();
};
