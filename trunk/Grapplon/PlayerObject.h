#pragma once

#include "BaseMovableObject.h"
#include "WiimoteListener.h"

#include <ode/ode.h>

class CParticleEmitter;

class CHook;

class CPlayerObject : public CBaseMovableObject, public IWiimoteListener
{
private:
	float y,p,r;
	CAnimatedTexture *m_pRadius;
	int m_iPlayer;
	CHook *m_pHook;

	float timeSinceNoInput;
	float m_fRespawnTime;

	CParticleEmitter *m_pThrusterLeft, *m_pThrusterRight;

	void Respawn();

public:
	CPlayerObject( int iPlayer );
	virtual ~CPlayerObject();

	virtual bool HandleWiimoteEvent( wiimote_t* pWiimoteEvent );
	virtual void Update( float fTime );
	virtual void Render();
	virtual void SetPosition( float fX, float fY );
	virtual void SetPosition( Vector pos );

	virtual void OnDie( CBaseObject *m_pKiller );
};
