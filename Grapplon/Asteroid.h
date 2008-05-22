#pragma once

#include "Planet.h"
#include "PlanetaryData.h"
#include <time.h>

class CPlayerObject;

enum AsteroidType { LARGE, SMALL };

class CAsteroid : public CPlanet
{
private:
	float m_fRespawnTime;

public:
	CAsteroid(PlanetaryData &data);
	virtual ~CAsteroid();
	
	bool m_bIsGrabable;
	AsteroidType m_eAsteroidType;

	CPlayerObject* m_pThrowingPlayer;
	CPlayerObject* m_pHoldingPlayer;
	time_t m_fThrowTime;
	int m_iMilliSecsInOrbit;
	int m_iWallBounces;
	float m_fBounceToggleTime;

	virtual void Update( float fTime );
	virtual void Render();
	void Respawn();
	void Explode();
	void LeaveField();
	void OnPlanetCollide(CBaseObject *pOther, Vector force);
	void CollideWith(CBaseObject *pOther, Vector force);
	void Split();
};
