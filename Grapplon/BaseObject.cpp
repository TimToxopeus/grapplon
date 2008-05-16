#include "BaseObject.h"
#include "Texture.h"
#include "ODEManager.h"
#include "ode/objects.h"
#include "AnimatedTexture.h"
#include "LogManager.h"

CBaseObject::CBaseObject()
{
	m_fAngle = 0.0f;
	m_pImage = NULL;
	SetDepth( -2.0f );
	m_fGravitationalConstant = 0.0f;

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this,m_oPhysicsData);
	SetMass( 10.0f );

	m_iHitpoints = 10000;
	m_iMaxHitpoints = 10000;
	m_fScale = 1.0f;
}

CBaseObject::~CBaseObject(){
	if(m_pImage) delete m_pImage;
}

void CBaseObject::Render()
{
	SDL_Rect target, size;
	size = m_pImage->GetSize();
	target.w = (int)((float)size.w * m_fScale);
	target.h = (int)((float)size.h * m_fScale);
	target.x = (int)GetX() - (target.w / 2);
	target.y = (int)GetY() - (target.h / 2);
	RenderQuad( target, m_pImage, m_fAngle );
}

void CBaseObject::Update( float fTime )
{
	Vector normal;
/*	if ( m_oPhysicsData.body->posr.pos[0] < -4096 )
		m_bDeleteMe = true;
	if ( m_oPhysicsData.body->posr.pos[0] > 4096 )
		m_bDeleteMe = true;
	if ( m_oPhysicsData.body->posr.pos[1] < -3072 )
		m_bDeleteMe = true;
	if ( m_oPhysicsData.body->posr.pos[1] > 3072 )
		m_bDeleteMe = true;*/
/*	normal.Normalize();
	Vector vel = Vector( m_oPhysicsData.body->lvel );
	vel.Mirror( normal ).CopyInto( m_oPhysicsData.body->lvel );*/

	m_pImage->UpdateFrame( fTime );
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
	return dBodyGetPosition(m_oPhysicsData.body);
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

void CBaseObject::SetMass( float fMass, bool perminent )
{
	dMass mass; 
	dMassSetBox(&mass, 1, 1, 1, 1); 
	dMassAdjust(&mass, fMass); 
	dBodySetMass(m_oPhysicsData.body, &mass);
	
	if(perminent)
		m_oPhysicsData.m_fMass = fMass;
}

void CBaseObject::ResetMass(){
	SetMass(m_oPhysicsData.m_fMass, true);
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

void inline CBaseObject::ApplyForceFront()
{
	dBodyAddForceAtRelPos(m_oPhysicsData.body, frontForce[0], frontForce[1], 0.0f, 0.0f, 0.0f, 0.0f);
}


void CBaseObject::SetForceFront( Vector f )
{
	frontForce = f;
}

void CBaseObject::SetForce( Vector f )
{
	dBodySetForce(m_oPhysicsData.body, f[0], f[1], 0.0f);
}

Vector CBaseObject::GetForwardVector()
{
	Vector v = Vector( cos(DEGTORAD(m_fAngle)), sin(DEGTORAD(m_fAngle)), 0.0f );
	v.Normalize();
	return v;
}

void CBaseObject::CollideWith( CBaseObject *pOther, Vector force )
{
	int iOldHitpoints = m_iHitpoints;
	m_iHitpoints -= (int)force.Length();
	if ( m_iHitpoints <= 0 )
		m_iHitpoints = 0;

	if ( m_iHitpoints == 0 && iOldHitpoints > 0 )
	{
		CLogManager::Instance()->LogMessage( "Object died" );
	}
}
