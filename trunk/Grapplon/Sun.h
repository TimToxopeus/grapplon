#pragma once

#include "Planet.h"
//#include "PlanetaryData.h"
//#include <time.h>

class CSun : public CPlanet
{
private:
//	PlanetaryData *data;
	float m_fRespawnTime;

public:
	CSun(PlanetaryData &data);
	virtual ~CSun();
	
	void SetOrbitJoint( dJointID joint ) { m_oPhysicsData.planetData->orbitJoint = joint; }

	virtual void Render();
};
