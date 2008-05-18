#include "Planet.h"
#include "ODEManager.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"
#include "ParticleSystemManager.h"

CPlanet::CPlanet(PlanetaryData &data)
	: m_pThrowingPlayer(NULL), m_fThrowTime(0)
{
	if(data.gravconst == 0.0f) 
		m_eType = ASTEROID;
	else
		m_eType = PLANET;
		

	std::string image = "media/scripts/" + data.image + ".txt";
	m_pImage = new CAnimatedTexture(image);

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this, &m_oPhysicsData, (float)data.radius);

	m_oPhysicsData.planetData = &data;

	SetDepth( -2.0f );
	SetMass( (float)m_oPhysicsData.planetData->mass );
	SetPosition( m_oPhysicsData.planetData->position );

	m_oPhysicsData.m_fGravConst = m_oPhysicsData.planetData->gravconst;
	m_oPhysicsData.m_bAffectedByGravity = false;
	m_fSecondaryScale = data.scale;

	m_oPhysicsData.planetData->bIsOrbitting = false;

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
	if ( m_oPhysicsData.planetData->orbitJoint && m_oPhysicsData.planetData->bIsOrbitting )
	{
		
		Vector pos = dBodyGetPosition( dJointGetBody( m_oPhysicsData.planetData->orbitJoint, 0 ) );
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

	//Vector pos = m_oPhysicsData.m_pOwner->GetPosition();
	//if ( pos[0] < -4096 || pos[0] > 4096 || pos[1] < -3072 || pos[1] > 3072)
	//	m_bDeleteMe = true;

	CBaseObject::Update( fTime );
}
