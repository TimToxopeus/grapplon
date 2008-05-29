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
	CAnimatedTexture* m_pImageDamage;
	CAnimatedTexture *m_pExplosion;
	int m_iPlayer;
	float m_fExplosionAngle;
	CHook *m_pHook;


	float timeSinceNoInput;
	float m_fRespawnTime;
	bool  m_bHandleWiiMoteEvents;
	CParticleEmitter *m_pThrusterLeft, *m_pThrusterRight;

	void Respawn();

public:

	CPlayerObject( int iPlayer );
	virtual ~CPlayerObject();
	
	int m_iScore;

	virtual bool HandleWiimoteEvent( wiimote_t* pWiimoteEvent );
	virtual void Update( float fTime );
	virtual void Render();
	virtual void SetPosition( float fX, float fY );
	virtual void SetPosition( Vector pos );
	virtual void CollideWith( CBaseObject *pOther);

	virtual void OnDie( CBaseObject *m_pKiller );


};
