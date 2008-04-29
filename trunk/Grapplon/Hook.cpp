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
	m_oPhysicsData.m_bIsHook = true;

	m_oPhysicsData.ToggleIgnore( pOwner->GetPhysicsData() );
}

void CHook::Disconnect()
{
	m_bDisconnected = true;
	//m_oPhysicsData.m_bAffectedByGravity = true;
	m_oPhysicsData.m_bHasCollision = true;
}

void CHook::Reconnect()
{
	m_bDisconnected = false;
	m_oPhysicsData.m_bAffectedByGravity = false;
	m_oPhysicsData.m_bHasCollision = false;

	if ( m_oPhysicsData.m_pGrabbedObject )
	{
		m_oPhysicsData.m_pGrabbedObject->ToggleIgnore( m_pOwner->GetPhysicsData() );
		m_oPhysicsData.m_pGrabbedObject->m_bHasCollision = true;
		m_oPhysicsData.m_pGrabbedObject->m_bAffectedByGravity = true;
		dMass mass; 
		dMassSetBox(&mass, 1, 1, 1, 1); 
		dMassAdjust(&mass, m_oPhysicsData.m_pGrabbedObject->m_fMass); 
		dBodySetMass(m_oPhysicsData.m_pGrabbedObject->body, &mass);
		Vector(m_pOwner->GetBody()->lvel).CopyInto( m_oPhysicsData.m_pGrabbedObject->body->lvel );
		m_oPhysicsData.m_pGrabbedObject = 0;
	}
	if ( m_oPhysicsData.m_oHookGrabJoint )
	{
		CODEManager::Instance()->DestroyJoint( m_oPhysicsData.m_oHookGrabJoint );
		m_oPhysicsData.m_oHookGrabJoint = 0;
	}

	Vector n;
	n.CopyInto( m_oPhysicsData.body->lvel );
	n.CopyInto( m_oPhysicsData.body->avel );
}

void CHook::Update( float fTime )
{
//	m_fAngle = GetPosition().CalculateAngle( GetPosition() + Vector(m_oPhysicsData.body->lvel) );
	CBaseMovableObject::Update(fTime);
}
