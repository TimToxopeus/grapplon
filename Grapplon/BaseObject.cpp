#include "BaseObject.h"
#include "Texture.h"
#include "ODEManager.h"
#include "ode/objects.h"
#include "Vector.h"

#pragma warning(disable:4244)

CBaseObject::CBaseObject()
{
	m_fAngle = 0.0f;
	m_pImage = NULL;
	SetDepth( -2.0f );

	m_oBody = CODEManager::Instance()->CreateBody();
	Vector v( m_oBody->posr.pos );
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
	dBodySetPosition(m_oBody, fX, fY, GetDepth());
}

dReal *CBaseObject::GetPosition()
{
	return m_oBody->posr.pos;
}

float CBaseObject::GetX()
{
	return m_oBody->posr.pos[0];
}

float CBaseObject::GetY()
{
	return m_oBody->posr.pos[1];
}

void CBaseObject::SetRotation( float fAngle )
{
	m_fAngle = fAngle;
}

float CBaseObject::GetRotation()
{
	return m_fAngle;
}
