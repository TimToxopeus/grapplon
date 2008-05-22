#include "Asteroid.h"
#include "ODEManager.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"
#include "ParticleSystemManager.h"
#include "Renderer.h"

CAsteroid::CAsteroid(PlanetaryData &data) 
	: CPlanet(data), m_pThrowingPlayer(NULL), m_fThrowTime(0), m_iWallBounces(0), m_bIsGrabable(true), m_fRespawnTime(0.0f), m_fBounceToggleTime(0.0f)
{
	m_eType = ASTEROID;
}

CAsteroid::~CAsteroid(){}

void CAsteroid::Render()
{
	CBaseObject::Render();
}

void CAsteroid::OnPlanetCollide(CBaseObject *pOther, Vector force)
{
	if(!this->m_bIsGrabable) return;
	
	if(m_eAsteroidType == LARGE)
	{
		Explode();				// Todo Split
	}
	else
	{
		Explode();
	}

}

void CAsteroid::LeaveField()
{
	m_fRespawnTime = 2.0f;
	m_fInvincibleTime = 2.0f;
	m_bIsGrabable = false;
	//TODO: speel animatie voor 1 seconden;
}

void CAsteroid::Explode()
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

void CAsteroid::Respawn()
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

void CAsteroid::Update( float fTime )
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

	CPlanet::Update( fTime );
}

void CAsteroid::CollideWith(CBaseObject *pOther, Vector force)
{
	if(pOther->getType() == ORDINARY || pOther->getType() == SUN || pOther->getType() == ICE || pOther->getType() == BROKEN)
	{
		OnPlanetCollide(pOther, force);
	}


}