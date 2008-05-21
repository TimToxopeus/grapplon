#include "Planet.h"
#include "ODEManager.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"
#include "ParticleSystemManager.h"
#include "Renderer.h"

CPlanet::CPlanet(PlanetaryData &data)
	: m_pThrowingPlayer(NULL), m_fThrowTime(0), m_iWallBounces(0), m_bIsGrabable(true), m_fRespawnTime(0.0f), m_fBounceToggleTime(0.0f)

{
	if(data.gravconst == 0.0f) 
		m_eType = ASTEROID;
	else
		m_eType = PLANET;
		

	std::string image = "media/scripts/texture_" + data.image + ".txt";
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

void CPlanet::OnPlanetCollide(CBaseObject *pOther, Vector force)
{
	if(m_eAsteroidType == LARGE)
	{
		
		Explode();				// Todo Split

	}
	else
	{
		Explode();
	}

}

void CPlanet::LeaveField()
{
	m_fRespawnTime = 2.0f;
	m_fInvincibleTime = 2.0f;
	m_bIsGrabable = false;
	//TODO: speel animatie voor 1 seconden;
}

void CPlanet::Explode()
{
	Vector zeroVec;

	this->SetLinVelocity(zeroVec);
	this->SetAngVelocity(zeroVec);
	this->SetForce(zeroVec);

	m_fRespawnTime = 2.0f;
	m_fInvincibleTime = 2.0f;
	m_bIsGrabable = false;
	//TODO: speel animatie van explosie voor 1 seconden
}

void CPlanet::Respawn()
{
	int x, y;

	CRenderer *pRenderer = CRenderer::Instance();
	do
	{
		x = rand()%4000 - 2000;
		y = rand()%3000 - 1500;
	} while ( pRenderer->ObjectsInRange( x, y, (int) m_oPhysicsData.m_fRadius ) );

	Vector v = Vector( (float)x, (float)y, 0.0f );
	SetPosition( v );
	Vector n;
	m_oPhysicsData.m_pOwner->SetLinVelocity(n);
	m_oPhysicsData.m_pOwner->SetAngVelocity(n);
	SetForce(n);

	m_fThrowTime = -1;
	m_iMilliSecsInOrbit = 0;
	m_iWallBounces = 0;
	m_pHoldingPlayer = NULL;
	m_pThrowingPlayer = NULL;
	SetDepth(-1.0);
	SetAlpha(1.0);
	this->m_bIsGrabable = true;

}

void CPlanet::Update( float fTime )
{

	if(m_fBounceToggleTime > 0.0001f){
		m_fBounceToggleTime -= fTime;
	}

	if(m_fRespawnTime > 0.0001f)					// Still respawning
	{
		m_fRespawnTime -= fTime;

		if(m_fRespawnTime < 1.0f && m_fRespawnTime + fTime > 1.0f)			// Herpositionering op 2 seconden mark
		{
			Respawn();
		}
	}
	else
	{
		m_bIsGrabable = true;
	}


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

void CPlanet::CollideWith(CBaseObject *pOther, Vector force)
{
	if(pOther->getType() == PLANET)
	{
		OnPlanetCollide(pOther, force);
	}


}