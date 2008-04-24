#pragma once

#include "BaseObject.h"

class CPlanet : public CBaseObject
{
private:
//	float y,p,r;

public:
	CPlanet(const float& fMass);
	virtual ~CPlanet();

	virtual void Update( float fTime );
	virtual void Render();
};
