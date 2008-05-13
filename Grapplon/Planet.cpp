#include "Planet.h"
#include "ODEManager.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"

#include "ode/joint.h"

#pragma warning(disable:4244)

CPlanet::CPlanet(PlanetaryData &data)
{
	m_eType = PLANET;
	std::string image = "media/scripts/" + data.image + ".txt";
	m_pImage = new CAnimatedTexture(image);

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this,m_oPhysicsData, (float)data.radius);

	m_oPhysicsData.planetData = &data;

	SetDepth( -2.0f );
	SetMass( m_oPhysicsData.planetData->mass );
	SetPosition( m_oPhysicsData.planetData->position );

	m_oPhysicsData.m_fGravConst = m_oPhysicsData.planetData->gravconst;
	m_oPhysicsData.m_bAffectedByGravity = false;
}

CPlanet::~CPlanet()
{
	delete m_pImage;
}


void CPlanet::Render()
{
	CBaseObject::Render();
}

void CPlanet::Update( float fTime )
{
	if ( m_oPhysicsData.planetData->orbitJoint )
	{
		Vector pos = Vector(m_oPhysicsData.planetData->orbitJoint->node[0].body->posr.pos);
		float angle = GetPosition().CalculateAngle( pos ) - 90.0f;
		angle = DEGTORAD(angle);
		Vector dir = Vector( cos(angle), sin(angle), 0 );
		dir.Normalize();
		dir *= m_oPhysicsData.planetData->orbitSpeed;
		dBodySetLinearVel( m_oPhysicsData.body, dir[0], dir[1], 0 );
	}
	CBaseObject::Update( fTime );
}
