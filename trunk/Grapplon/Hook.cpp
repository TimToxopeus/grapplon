#include "Hook.h"
#include "ChainLink.h"
#include "PlayerObject.h"

#include "ODEManager.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"

#define LINK_THICK 15.0f
#define LINK_LENGTH 5.0f
#define LINK_AMOUNT 16
#define LINK_MOVE 2
#define CENT_DIST -18
#define CFM 0.001f
#define ERP 0.8f



CHook::CHook( CPlayerObject *pOwner )
{
	m_pOwner = pOwner;
	m_pImage = new CAnimatedTexture("media/scripts/hook.txt");
	SetDepth( -1.1f );
	m_eHookState = CONNECTED;
	startUp = 0.0f;

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this,m_oPhysicsData, 32.0f);
	m_oPhysicsData.m_bAffectedByGravity = false;
	m_oPhysicsData.m_bHasCollision = false;
	//m_oPhysicsData.m_fAirDragConst = 1.0f;  //0.5
	m_oPhysicsData.m_fAirDragConst = 0.5f;  //0.5
		
	
	m_oPhysicsData.m_bIsHook = true;
	//this->SetMass(1.0f);  //0.5
	this->SetMass(1.0f);  //0.5

	m_oPhysicsData.ToggleIgnore( pOwner->GetPhysicsData() );

	const float stop = 100.0f; 
	const float fmax = 100000.0f; 

	Vector shipPosition = dBodyGetPosition( m_pOwner->GetBody() );
	Vector forward = m_pOwner->GetForwardVector();
	this->SetPosition(shipPosition + forward*CENT_DIST);

	chainJoints = dJointGroupCreate(LINK_AMOUNT * 2 + 1);
	
	dJointID curJointID;
	CChainLink* curLink;
	dBodyID prevBodyID = m_pOwner->GetBody();
	Vector curPosition;
	Vector anchorPoint;

	for(int i = 0; i < LINK_AMOUNT * 2; i++){

		curLink = new CChainLink(pOwner);
		chainLinks.push_back( curLink );


		//if(i < LINK_AMOUNT * 2 - 2){
			curPosition = shipPosition + Vector(LINK_LENGTH / 2, 0.0f, 0.0f);
			anchorPoint = (i % 2 == 0 ? shipPosition : shipPosition + Vector(LINK_LENGTH, 0.0f, 0.0f));
		//} else {
		//	int num = i - (LINK_AMOUNT * 2 - 2);
		//	curPosition = shipPosition + forward * (num * LINK_LENGTH + LINK_LENGTH / 2, 0.0f);
		//	anchorPoint = shipPosition + forward * (num * LINK_LENGTH);
		//}



		dBodySetPosition( curLink->GetBody(), curPosition[0], curPosition[1], curPosition[2] );

		curJointID = dJointCreateHinge(ode->getWorld(), chainJoints);
		dJointAttach(curJointID, prevBodyID, curLink->GetBody());
		dJointSetHingeAnchor(curJointID, anchorPoint[0], anchorPoint[1], anchorPoint[2]);
		dJointSetHingeAxis(curJointID, 0, 0, 1);
		
		dJointSetHingeParam( curJointID, dParamLoStop, -stop ); 
		dJointSetHingeParam( curJointID, dParamHiStop, stop ); 
		dJointSetHingeParam( curJointID, dParamVel, 0 ); 
		dJointSetHingeParam( curJointID, dParamFMax, fmax ); 
		dJointSetHingeParam( curJointID, dParamBounce, 0 ); 
		dJointSetHingeParam( curJointID, dParamStopCFM, CFM ); 
		dJointSetHingeParam( curJointID, dParamStopERP, ERP ); 

		prevBodyID = curLink->GetBody();

	}



	// Atach Ship to last ChainLink
	m_pHookJoint = dJointCreateHinge(ode->getWorld(), 0);			// Don't place it in jointgroup!!
	dJointAttach(m_pHookJoint, prevBodyID, this->m_pOwner->GetBody());
	dJointSetHingeAnchor(m_pHookJoint, shipPosition[0] , shipPosition[1], shipPosition[2]);
	dJointSetHingeAxis(m_pHookJoint, 0, 0, 1);
	
	dJointSetHingeParam( m_pHookJoint, dParamLoStop, -stop ); 
	dJointSetHingeParam( m_pHookJoint, dParamHiStop, stop ); 
	dJointSetHingeParam( m_pHookJoint, dParamVel, 0 ); 
	dJointSetHingeParam( m_pHookJoint, dParamFMax, fmax ); 
	dJointSetHingeParam( m_pHookJoint, dParamBounce, 0 ); 
	dJointSetHingeParam( m_pHookJoint, dParamStopCFM, CFM ); 
	dJointSetHingeParam( m_pHookJoint, dParamStopERP, ERP ); 




/**


	Vector shipPosition = dBodyGetPosition( m_pOwner->GetBody() );
	chainJoints = dJointGroupCreate (LINK_AMOUNT + 1);

	this->SetPosition( shipPosition[0] + LINK_AMOUNT*LINK_LENGTH + LINK_LENGTH / 2, shipPosition[1]);

	dJointID curJointID;
	CChainLink* curLink;
	dBodyID  prevBodyID = m_pOwner->GetBody();

	for(int i = 0; i < LINK_AMOUNT; i++){

		curLink = new CChainLink(pOwner);
		chainLinks.push_back( curLink );
		
		dBodySetPosition( curLink->GetBody(), shipPosition[0] + i*LINK_LENGTH + LINK_LENGTH / 2, shipPosition[1], shipPosition[2] );
		//dQuaternion q; q[0] = sqrt(0.5f); q[1] = sqrt(0.5f); q[2] = 0.0f; q[3] = 0.0f;
		//dBodySetQuaternion( curLink->GetBody(), q );

		curJointID = dJointCreateHinge(ode->getWorld(), chainJoints);
		dJointAttach(curJointID, prevBodyID, curLink->GetBody());
		dJointSetHingeAnchor(curJointID, shipPosition[0] + i * LINK_LENGTH, shipPosition[1], shipPosition[2]);
		dJointSetHingeAxis(curJointID, 0, 0, 1);
		
		dJointSetHingeParam( curJointID, dParamLoStop, -stop ); 
		dJointSetHingeParam( curJointID, dParamHiStop, stop ); 
		dJointSetHingeParam( curJointID, dParamVel, 0 ); 
		dJointSetHingeParam( curJointID, dParamFMax, fmax ); 
		dJointSetHingeParam( curJointID, dParamBounce, 0 ); 
		dJointSetHingeParam( curJointID, dParamStopCFM, cfm ); 
		dJointSetHingeParam( curJointID, dParamStopERP, erp ); 

		prevBodyID = curLink->GetBody();

	}


	curJointID = dJointCreateHinge(ode->getWorld(), chainJoints);
	dJointAttach(curJointID, prevBodyID, m_oPhysicsData.body);
	dJointSetHingeAnchor(curJointID, shipPosition[0] + LINK_AMOUNT * LINK_LENGTH, shipPosition[1], shipPosition[2]);
	dJointSetHingeAxis(curJointID, 0, 0, 1);
	
	dJointSetHingeParam( curJointID, dParamLoStop, -stop ); 
	dJointSetHingeParam( curJointID, dParamHiStop, stop ); 
	dJointSetHingeParam( curJointID, dParamVel, 0 ); 
	dJointSetHingeParam( curJointID, dParamFMax, fmax ); 
	dJointSetHingeParam( curJointID, dParamBounce, 0 ); 
	dJointSetHingeParam( curJointID, dParamStopCFM, cfm ); 
	dJointSetHingeParam( curJointID, dParamStopERP, erp ); 


	// Atach hinge between ship and hook
	hookJoint = dJointCreateBall(ode->getWorld(), chainJoints);
	dJointAttach(hookJoint, m_pOwner->GetBody(), m_oPhysicsData.body);
	dJointSetBallAnchor(hookJoint, shipPosition[0], shipPosition[1], shipPosition[2]);
	
	//dJointSetBallParam( hookJoint, dParamLoStop, -stop ); 
	//dJointSetBallParam( hookJoint, dParamHiStop, stop ); 
	//dJointSetBallParam( hookJoint, dParamVel, 0 ); 
	dJointSetBallParam( hookJoint, dParamFMax, fmax ); 
	dJointSetBallParam( hookJoint, dParamBounce, 0 ); 
	dJointSetBallParam( hookJoint, dParamStopCFM, cfm ); 
	dJointSetBallParam( hookJoint, dParamStopERP, erp ); 

*/
}

CHook::~CHook()
{
	delete m_pImage;
}

void CHook::Eject()
{
	if(m_eHookState == CONNECTED){

		m_eHookState = HOMING;
		dBodyID lastChainLink = dJointGetBody(m_pHookJoint, 0);
		dJointDestroy(m_pHookJoint);
		
		Vector hookPos = this->GetPosition();

		m_pHookJoint = dJointCreateHinge(CODEManager::Instance()->getWorld(), 0);		// Don't place it in jointgroup!!
		dJointAttach(m_pHookJoint, lastChainLink, m_oPhysicsData.body);
		dJointSetHingeAnchor(m_pHookJoint, hookPos[0] , hookPos[1], hookPos[2]);
		dJointSetHingeAxis(m_pHookJoint, 0, 0, 1);
		
		dJointSetHingeParam( m_pHookJoint, dParamVel, 0 ); 
		dJointSetHingeParam( m_pHookJoint, dParamBounce, 0 ); 
		dJointSetHingeParam( m_pHookJoint, dParamStopCFM, CFM ); 
		dJointSetHingeParam( m_pHookJoint, dParamStopERP, ERP ); 

		m_oPhysicsData.m_bHasCollision = true;
		
		Vector shipFor = m_pOwner->GetForwardVector() * 10000000.0f;
		dBodyAddForce(m_oPhysicsData.body, shipFor[0], shipFor[1], 0.0f);
	}

}

void CHook::Reconnect()
{
	m_eHookState = CONNECTED;
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

void CHook::AddChainForce(float x_force, float y_force)
{
	frontForce = Vector(x_force, -y_force, 0.0f);
}

void CHook::ApplyForceFront()
{

	if(m_eHookState == DISCONNECTED){

		Vector shipPos = dBodyGetPosition( this->m_pOwner->GetBody() );
		Vector hookPos = dBodyGetPosition( this->GetBody() );
		Vector radial  = hookPos - shipPos;
		radial.Normalize();
		Vector tangent(radial[1], -radial[0], 0.0f);
		
		if( frontForce[0] * frontForce[1] != 0.0f ){
			float force = frontForce.Length();
			//float inprod = tangent.DotProduct( frontForce.GetNormalized() );
			float inprod = tangent.DotProduct( frontForce );
			

			//if(inprod > 0){
				tangent *= force * 50;
				//tangent *= angle * force * 20;
			//} else {
			//	tangent *= -10;
			//}

			if(tangent.Length() > 20.0f){
				tangent.Normalize();
				tangent *= 20.0f;
			}
		}
		

		//Vector tmp = radial*-1 - tangent;
		//Vector tmp2 = tangent; //tangent + tmp * 0.5;
		//tmp2.Normalize();

		//float dot = tangent.DotProduct(accel);
		//tangent *= dot;

		
		//this->chainLinks[LINK_MOVE]->AddForce(Vector(x_force, y_force, 0.0f));
		//this->AddForce(Vector(x_force * 10.0f, y_force * 10.0f, 0.0f));

		dBodyAddForce(m_oPhysicsData.body, tangent[0] * 150.0f, tangent[1] * 150.0f, 0.0f);
	} else if (m_eHookState == HOMING) {


	}
}

void CHook::Update( float fTime )
{
	//m_fAngle = GetPosition().CalculateAngle( GetPosition() + Vector(m_oPhysicsData.body->lvel) );

	if(m_eHookState == CONNECTED){
		m_fAngle = m_pOwner->GetRotation();
		Vector shipFor = m_pOwner->GetForwardVector();
		Vector shipPos = m_pOwner->GetPosition();
		this->SetPosition(shipPos + shipFor * CENT_DIST);
	}

/*
	Vector shipPosition = dBodyGetPosition( m_pOwner->GetBody() );
	Vector hookPosition = dBodyGetPosition( m_oPhysicsData.body );

	Vector dir = shipPosition - hookPosition;
	float length = dir.Length();
	float cor = ( LINK_AMOUNT*LINK_LENGTH + LINK_LENGTH / 2 ) / length;
	Vector newPos = shipPosition + dir * cor;

	this->SetPosition(newPos[0], newPos[1]);
*/	

	CBaseMovableObject::Update(fTime);
}