#pragma once

#include "BaseObject.h"
#include "PlanetaryData.h"
#include <time.h>

class CParticleEmitter;
class CPlayerObject;

class CPlanet : public CBaseObject
{
private:
//	PlanetaryData *data;
	CParticleEmitter *m_pEmitter;

public:
	CPlanet(PlanetaryData &data);
	virtual ~CPlanet();

	CPlayerObject* m_pThrowingPlayer;
	time_t m_fThrowTime;
	int m_iMilliSecsInOrbit;

	void SetOrbitJoint( dJointID joint ) { m_oPhysicsData.planetData->orbitJoint = joint; }

	virtual void Update( float fTime );
	virtual void Render();
};
