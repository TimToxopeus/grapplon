#pragma once

#include "BaseObject.h"
#include "PlanetaryData.h"

class CPlanet : public CBaseObject
{
private:
	PlanetaryData *data;

public:
	CPlanet(PlanetaryData &data);
	virtual ~CPlanet();

	void SetOrbitJoint( dJointID joint ) { data->orbitJoint = joint; }

	virtual void Update( float fTime );
	virtual void Render();
};
