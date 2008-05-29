#include "PowerUp.h"
#include "AnimatedTexture.h"
#include "ODEManager.h"

CPowerUp::CPowerUp( int type )
{
	m_eType = POWERUP;
	m_iType = type;

	m_pImage = new CAnimatedTexture("media/scripts/texture_powerups.txt");
	m_pImage->SetAnimation(type);

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this, &m_oPhysicsData, 30.0f);
	SetMass( 1000.0f );
	m_oPhysicsData.m_bAffectedByGravity = false;
}

void CPowerUp::Render()
{
	CBaseObject::Render();
}

void CPowerUp::CollideWith(CBaseObject *pOther)
{
}
