#include "Hook.h"
#include "ChainLink.h"
#include "PlayerObject.h"

#include "ODEManager.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"

#define LINK_THICK 10.0f
#define LINK_LENGTH 20.0f
#define LINK_AMOUNT 10


CHook::CHook( CPlayerObject *pOwner )
{
	m_pOwner = pOwner;
	m_pImage = new CAnimatedTexture("media/scripts/hook.txt");
	SetDepth( -1.1f );
	m_bDisconnected = false;

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this,m_oPhysicsData, 32.0f);
	m_oPhysicsData.m_bAffectedByGravity = false;
	m_oPhysicsData.m_bHasCollision = false;
	m_oPhysicsData.m_bIsHook = true;

	m_oPhysicsData.ToggleIgnore( pOwner->GetPhysicsData() );


	const float stop = 0.01f; 
	const float fmax = 1000; 
	const float cfm = 0.01f; 
	const float erp = 0.08f; 


	CChainLink* curLink;

	dJointID curJointID;
	dBodyID  prevBodyID = m_pOwner->GetBody();

	Vector shipPosition = dBodyGetPosition( m_pOwner->GetBody() );

	chainJoints = dJointGroupCreate (LINK_AMOUNT + 1);

	for(int i = 0; i < LINK_AMOUNT; i++){

		curLink = new CChainLink(pOwner);
		chainLinks.push_back( curLink );
		
		dBodySetPosition( curLink->GetBody(), shipPosition[0] + float(i)*LINK_LENGTH + LINK_LENGTH / float(2), shipPosition[1], shipPosition[2] );
		//dQuaternion q; q[0] = sqrt(0.5f); q[1] = sqrt(0.5f); q[2] = 0.0f; q[3] = 0.0f;
		//dBodySetQuaternion( curLink->GetBody(), q );

		curJointID = dJointCreateUniversal(ode->getWorld(), chainJoints);
		dJointAttach(curJointID, prevBodyID, curLink->GetBody());
		dJointSetUniversalAnchor(curJointID, shipPosition[0] + i * LINK_LENGTH, shipPosition[1], shipPosition[2]);
		
		dJointSetUniversalAxis1( curJointID, 1, 0, 0 ); 
		dJointSetUniversalAxis2( curJointID, 0, 1, 0 ); 

		dJointSetUniversalParam( curJointID, dParamLoStop, -stop ); 
		dJointSetUniversalParam( curJointID, dParamHiStop, stop ); 
		dJointSetUniversalParam( curJointID, dParamVel, 0 ); 
		dJointSetUniversalParam( curJointID, dParamFMax, fmax ); 
		dJointSetUniversalParam( curJointID, dParamBounce, 0 ); 
		dJointSetUniversalParam( curJointID, dParamStopCFM, cfm ); 
		dJointSetUniversalParam( curJointID, dParamStopERP, erp ); 


		dJointSetUniversalParam( curJointID, dParamLoStop2, -stop ); 
		dJointSetUniversalParam( curJointID, dParamHiStop2, stop ); 
		dJointSetUniversalParam( curJointID, dParamVel2, 0 ); 
		dJointSetUniversalParam( curJointID, dParamFMax2, fmax ); 
		dJointSetUniversalParam( curJointID, dParamBounce2, 0 ); 
		dJointSetUniversalParam( curJointID, dParamStopCFM2, cfm ); 
		dJointSetUniversalParam( curJointID, dParamStopERP2, erp ); 

		prevBodyID = curLink->GetBody();

	}




}

CHook::~CHook()
{
	delete m_pImage;
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

void CHook::AddRope()
{

}


void CHook::Update( float fTime )
{
//	m_fAngle = GetPosition().CalculateAngle( GetPosition() + Vector(m_oPhysicsData.body->lvel) );
	CBaseMovableObject::Update(fTime);
}
