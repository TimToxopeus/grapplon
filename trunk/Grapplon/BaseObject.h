#pragma once

#include "ActiveObject.h"
#include "ode/ode.h"
#include "Vector.h"
#include "PhysicsData.h"

class CTexture; // Forward declaration

class CBaseObject : public IActiveObject
{
protected:
	PhysicsData m_oPhysicsData;
	float m_fAngle;
	float m_fGravitationalConstant;
	CTexture *m_pImage;

public:
	CBaseObject();
	virtual ~CBaseObject() {}

	void Render();
	void Update( float fTime );

	void SetPosition( float fX, float fY );
	void SetPosition( Vector pos );
	Vector GetPosition();
	float GetX();
	float GetY();
	void SetRotation( float fAngle );
	float GetRotation();
	dBodyID getBody() { return m_oPhysicsData.body; };

	void SetMass( float fMass );
	float GetMass();

	void SetGravitationalConstant( float fGravitationalConstant ) { m_oPhysicsData.m_fGravConst = fGravitationalConstant; }
	float GetGravitationalConstant() { return m_oPhysicsData.m_fGravConst; }

	void SetVelocity( Vector v );
	void AddForce( Vector f );
};
