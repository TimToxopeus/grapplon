#include "BaseObject.h"
#include "Texture.h"
#include "ODEManager.h"
#include "ode/objects.h"

#pragma warning(disable:4244)

CBaseObject::CBaseObject()
{
	m_fAngle = 0.0f;
	m_pImage = NULL;
	SetDepth( -2.0f );
	m_fGravitationalConstant = 0.0f;

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(m_oPhysicsData);
	SetMass( 10.0f );
}

void CBaseObject::Render()
{
	SDL_Rect target, size;
	size = m_pImage->GetSize();
	target.x = GetX() - (size.w / 2);
	target.y = GetY() - (size.h / 2);
	target.w = size.w;
	target.h = size.h;
	RenderQuad( target, m_pImage, m_fAngle );
}

void CBaseObject::Update( float fTime )
{
}

void CBaseObject::SetPosition( float fX, float fY )
{
	dBodySetPosition(m_oPhysicsData.body, fX, fY, 0.0f);
}

void CBaseObject::SetPosition( Vector pos )
{
	dBodySetPosition(m_oPhysicsData.body, pos[0], pos[1], 0.0f);
}

Vector CBaseObject::GetPosition()
{
	return Vector(m_oPhysicsData.body->posr.pos);
}

float CBaseObject::GetX()
{
	return m_oPhysicsData.body->posr.pos[0];
}

float CBaseObject::GetY()
{
	return m_oPhysicsData.body->posr.pos[1];
}

void CBaseObject::SetRotation( float fAngle )
{
	m_fAngle = fAngle;
}

float CBaseObject::GetRotation()
{
	return m_fAngle;
}

void CBaseObject::SetMass( float fMass )
{
	dMass mass; 
	dMassSetBox(&mass, 1, 1, 1, 1); 
	dMassAdjust(&mass, fMass); 

	dBodySetMass(m_oPhysicsData.body, &mass);  
}

float CBaseObject::GetMass()
{
	dMass mass;
	dBodyGetMass( m_oPhysicsData.body, &mass );
	return mass.mass;
}

void CBaseObject::SetVelocity( Vector v )
{
	v.CopyInto( m_oPhysicsData.body->lvel );
}

void CBaseObject::AddForce( Vector f )
{
	dBodyAddForce(m_oPhysicsData.body, f[0], f[1], 0.0f);
}
