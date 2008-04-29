#include "Planet.h"
#include "ODEManager.h"
#include "ResourceManager.h"
#include "Texture.h"

#include "ode/joint.h"

#pragma warning(disable:4244)

CPlanet::CPlanet(PlanetaryData &data)
{
	this->data = &data;

	std::string image = "media/images/" + data.image;
	m_pImage = (CTexture *)CResourceManager::Instance()->GetResource(image, RT_TEXTURE);

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(m_oPhysicsData, (float)data.radius);

	SetDepth( -2.0f );
	SetMass( this->data->mass );
	SetPosition( this->data->position );

	m_oPhysicsData.m_fGravConst = this->data->gravconst;
	m_oPhysicsData.m_bAffectedByGravity = false;
}

CPlanet::~CPlanet()
{
}


void CPlanet::Render()
{
	CBaseObject::Render();
}

void CPlanet::Update( float fTime )
{
	if ( data->orbitJoint )
	{
		Vector pos = Vector(data->orbitJoint->node[0].body->posr.pos);
		float angle = GetPosition().CalculateAngle( pos ) - 90.0f;
		angle = DEGTORAD(angle);
		Vector dir = Vector( cos(angle), sin(angle), 0 );
		dir.Normalize();
		dir *= data->orbitSpeed;
		dBodySetLinearVel( m_oPhysicsData.body, dir[0], dir[1], 0 );
	}
	CBaseObject::Update( fTime );
}
