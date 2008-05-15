#include <ode/ode.h>
#include <ode/mass.h>

#include "ChainLink.h"
#include "PlayerObject.h"

#include "ODEManager.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"

#define LINK_LENGTH 50.0f
#define LINK_MASS 0.1f

CChainLink::CChainLink( CPlayerObject *pOwner )
	: m_pOwner(pOwner)
{
	m_eType = CHAINLINK;
	m_pImage = new CAnimatedTexture("media/scripts/chain.txt");
	SetDepth( -1.1f );


	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this, m_oPhysicsData, 0.0f);
	m_oPhysicsData.m_fAirDragConst = 0.5f;
	m_oPhysicsData.m_bAffectedByGravity = false;
	m_oPhysicsData.m_bHasCollision = false;

	SetMass( 0.05f );

	m_oPhysicsData.ToggleIgnore( pOwner->GetPhysicsData() );

}

CChainLink::~CChainLink()
{
	delete m_pImage;
}



void CChainLink::Update( float fTime )
{
	CBaseMovableObject::Update(fTime);
}
