#include "Planet.h"
#include "ODEManager.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"
#include "ParticleSystemManager.h"

#include "ode/joint.h"

CPlanet::CPlanet(PlanetaryData &data)
{
	m_eType = PLANET;
	std::string image = "media/scripts/" + data.image + ".txt";
	m_pImage = new CAnimatedTexture(image);

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this,m_oPhysicsData, (float)data.radius);

	m_oPhysicsData.planetData = &data;

	SetDepth( -2.0f );
	SetMass( (float)m_oPhysicsData.planetData->mass );
	SetPosition( m_oPhysicsData.planetData->position );

	m_oPhysicsData.m_fGravConst = m_oPhysicsData.planetData->gravconst;
	m_oPhysicsData.m_bAffectedByGravity = false;
	m_fScale = data.scale;

	m_pEmitter = NULL;
	if ( data.emitter != "" )
	{
		if ( data.bNear )
			m_pEmitter = CParticleSystemManager::InstanceNear()->LoadEmitter( data.emitter );
		else
			m_pEmitter = CParticleSystemManager::InstanceFar()->LoadEmitter( data.emitter );
	}
}

CPlanet::~CPlanet(){}

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

	if ( m_pEmitter )
		m_pEmitter->SetPosition( GetPosition() );

	m_fAngle += (float)m_oPhysicsData.planetData->rotation * fTime;
	CBaseObject::Update( fTime );
}
