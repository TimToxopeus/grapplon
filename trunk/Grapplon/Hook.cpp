#include "Hook.h"
#include "PlayerObject.h"

#include "ODEManager.h"
#include "ResourceManager.h"
#include "Texture.h"

CHook::CHook( CPlayerObject *pOwner )
{
	m_pOwner = pOwner;
	m_pImage = (CTexture *)CResourceManager::Instance()->GetResource("media/images/hook.png", RT_TEXTURE);
	SetDepth( -1.1f );
	m_bDisconnected = false;

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(m_oPhysicsData, 32.0f);
	m_oPhysicsData.m_bAffectedByGravity = false;
	m_oPhysicsData.m_bHasCollision = false;

	m_oPhysicsData.ToggleIgnore( pOwner->GetPhysicsData() );
}

void CHook::Disconnect()
{
	m_bDisconnected = true;
	m_oPhysicsData.m_bAffectedByGravity = true;
	m_oPhysicsData.m_bHasCollision = true;
}

void CHook::Reconnect()
{
	m_bDisconnected = false;
	m_oPhysicsData.m_bAffectedByGravity = false;
	m_oPhysicsData.m_bHasCollision = false;

	Vector n;
	n.CopyInto( m_oPhysicsData.body->lvel );
}

void CHook::Update( float fTime )
{
	m_fAngle = GetPosition().CalculateAngle( GetPosition() + Vector(m_oPhysicsData.body->lvel) );
	CBaseMovableObject::Update(fTime);
}
