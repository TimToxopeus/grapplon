#include "Planet.h"
#include "ODEManager.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"
#include "ParticleSystemManager.h"
#include "Renderer.h"

CPlanet::CPlanet(PlanetaryData &data)
{
	m_eType = data.planetType;
	
	std::string image = "media/scripts/texture_" + data.image + ".txt";
	m_pImage = new CAnimatedTexture(image);

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this, &m_oPhysicsData, (float)data.radius);

	m_oPhysicsData.planetData = &data;

	orbitAngle  = (float) data.orbitAngle;
	orbitLength = (float) data.orbitLength;

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

	m_pOrbit = m_pGlow = NULL;
	if ( data.imageGlow != "" )
	{
		m_pGlow = new CAnimatedTexture( "media/scripts/texture_" + data.imageGlow + ".txt" );
	}
	if ( data.imageOrbit != "" )
	{
		m_pOrbit = new CAnimatedTexture( "media/scripts/texture_" + data.imageOrbit + ".txt" );
	}
}

CPlanet::~CPlanet()
{
	if ( m_pGlow ) delete m_pGlow;
	if ( m_pOrbit ) delete m_pOrbit;
}

void CPlanet::Render()
{
	SDL_Rect target;

	// Render orbit
	if ( m_pOrbit )
	{
		target = m_pOrbit->GetSize();
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);
		RenderQuad( target, m_pOrbit, 0 );
	}

	// Render glow
	if ( m_pGlow )
	{
		target = m_pGlow->GetSize();
		target.x = (int)GetX() - (target.w / 2);
		target.y = (int)GetY() - (target.h / 2);
		RenderQuad( target, m_pGlow, 0 );
	}

	// Render planet
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

	if ( m_pOrbit ) m_pOrbit->UpdateFrame( fTime );
	if ( m_pGlow ) m_pGlow->UpdateFrame( fTime );

	CBaseObject::Update( fTime );
}

