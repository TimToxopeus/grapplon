#pragma once

#include "BaseObject.h"
#include "PlanetaryData.h"
#include <time.h>

class CParticleEmitter;
class CPlayerObject;

enum AsteroidType { LARGE, SMALL };

class CPlanet : public CBaseObject
{
private:
//	PlanetaryData *data;
	CParticleEmitter *m_pEmitter;
	float m_fRespawnTime;

public:
	CPlanet(PlanetaryData &data);
	virtual ~CPlanet();
	
	bool m_bIsGrabable;
	AsteroidType m_eAsteroidType;

	CPlayerObject* m_pThrowingPlayer;
	CPlayerObject* m_pHoldingPlayer;
	time_t m_fThrowTime;
	int m_iMilliSecsInOrbit;
	int m_iWallBounces;

	void SetOrbitJoint( dJointID joint ) { m_oPhysicsData.planetData->orbitJoint = joint; }

	virtual void Update( float fTime );
	virtual void Render();
	void Respawn();
	void Explode();
	void LeaveField();
	void OnPlanetCollide(CBaseObject *pOther, Vector force);
	void CollideWith(CBaseObject *pOther, Vector force)
	void Split();
};
