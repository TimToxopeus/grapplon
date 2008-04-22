#pragma once

#include "ActiveObject.h"
#include "ode/ode.h"

class CTexture; // Forward declaration

class CBaseObject : public IActiveObject
{
protected:
	dBodyID m_oBody; // ODE Body
	float m_fAngle;
	CTexture *m_pImage;

public:
	CBaseObject();
	virtual ~CBaseObject() {}

	void Render();
	void Update( float fTime );

	void SetPosition( float fX, float fY );
	dReal *GetPosition();
	float GetX();
	float GetY();
	void SetRotation( float fAngle );
	float GetRotation();
};
