#pragma once

#include "BaseObject.h"
#include "PlanetaryData.h"
#include <time.h>

class CAnimatedTexture;
class CParticleEmitter;
class CPlayerObject;

class CPlanet : public CBaseObject
{
private:
//	PlanetaryData *data;
	CParticleEmitter *m_pEmitter;
	float m_fRespawnTime;

	CAnimatedTexture *m_pOrbit;
	CAnimatedTexture *m_pGlow;

public:
	CPlanet(PlanetaryData &data);
	virtual ~CPlanet();

	CPlanet* orbitOwner;
	float m_fOrbitAngle;
	float m_fOrbitLength;

	void SetOrbitJoint( dJointID joint ) { m_oPhysicsData.planetData->orbitJoint = joint; }

	virtual void Update( float fTime );
	virtual void Render();
};
