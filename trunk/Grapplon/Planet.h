#pragma once

#include "BaseObject.h"
#include "PlanetaryData.h"

class CParticleEmitter;

class CPlanet : public CBaseObject
{
private:
//	PlanetaryData *data;
	CParticleEmitter *m_pEmitter;

public:
	CPlanet(PlanetaryData &data);
	virtual ~CPlanet();

	void SetOrbitJoint( dJointID joint ) { m_oPhysicsData.planetData->orbitJoint = joint; }

	virtual void Update( float fTime );
	virtual void Render();
};
