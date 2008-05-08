#include <string>
#include <sstream>
#include "LogManager.h"

#include "Hook.h"
#include "ChainLink.h"
#include "PlayerObject.h"

#include "ODEManager.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"

#define LINK_THICK 15.0f
#define LINK_LENGTH 10.0f
#define LINK_AMOUNT 8
#define LINK_GRASP_CON 8
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
	isRadialCorrected = false;
	startUp = 0.0f;

	CODEManager* ode = CODEManager::Instance(); 
	ode->CreatePhysicsData(this,m_oPhysicsData, 32.0f);
	m_oPhysicsData.m_bAffectedByGravity = false;
	m_oPhysicsData.m_bHasCollision = false;
	m_oPhysicsData.m_fAirDragConst = 0.5f;  //0.5
		
	
	m_oPhysicsData.m_bIsHook = true;
	this->SetMass(1.0f);

	m_oHookGrabJoint    = dJointCreateHinge(ode->getWorld(), 0);
	m_oMiddleChainJoint = dJointCreateHinge(ode->getWorld(), 0);
	m_oAngleJoint       = dJointCreateHinge(ode->getWorld(), 0);

	PhysicsData *m_pGrabbedObject = NULL;


	m_oPhysicsData.ToggleIgnore( pOwner->GetPhysicsData() );

	const float stop = 100.0f; 
	const float fmax = 100000.0f; 

	Vector shipPosition = dBodyGetPosition( m_pOwner->GetBody() );
	Vector forward = m_pOwner->GetForwardVector();
	this->SetPosition(shipPosition + forward*CENT_DIST);

	//chainJoints = dJointGroupCreate(LINK_AMOUNT * 2 + 1);
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

		curJointID = dJointCreateHinge(ode->getWorld(), NULL);
		chainJoints.push_back(curJointID);
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
	m_pLastChainJoint = dJointCreateHinge(ode->getWorld(), 0);			// Don't place it in jointgroup!!
	chainJoints.push_back(curJointID);
	dJointAttach(m_pLastChainJoint, prevBodyID, this->m_pOwner->GetBody());
	dJointSetHingeAnchor(m_pLastChainJoint, shipPosition[0] , shipPosition[1], shipPosition[2]);
	dJointSetHingeAxis(m_pLastChainJoint, 0, 0, 1);
	
	dJointSetHingeParam( m_pLastChainJoint, dParamBounce, 0 ); 
	dJointSetHingeParam( m_pLastChainJoint, dParamStopCFM, CFM ); 
	dJointSetHingeParam( m_pLastChainJoint, dParamStopERP, ERP ); 


}

CHook::~CHook()
{
	delete m_pImage;
}

void CHook::Grasp(PhysicsData* toGrasp){

	m_eHookState = GRASPING;

	// Clear previous joints if applicable
	if ( toGrasp->m_bIsPlanet && toGrasp->planetData->orbitJoint)
	{
		dJointAttach( toGrasp->planetData->orbitJoint, 0, 0 );
		toGrasp->planetData->orbitJoint = NULL;
	}

	// Create grab joint
	dJointAttach( m_oHookGrabJoint, m_oPhysicsData.body, toGrasp->body );
	toGrasp->m_bHasCollision = false;
	toGrasp->m_bAffectedByGravity = false;

	// Set the mass to 1 to remove movement lag
	dMass mass; 
	dMassSetBox(&mass, 1, 1, 1, 1); 
	dMassAdjust(&mass, 0.5); 
	dBodySetMass(toGrasp->body, &mass);

	m_pGrabbedObject = toGrasp;

	Vector shipPos = m_pOwner->GetPosition(); //+ (m_pOwner->GetForwardVector() * CENT_DIST);

	// Joint between ship and before-middle link
	dJointAttach( chainJoints[LINK_GRASP_CON], chainLinks[LINK_GRASP_CON - 1]->GetBody(), this->m_pOwner->GetBody() );
	dJointSetHingeAnchor(chainJoints[LINK_GRASP_CON], shipPos[0] , shipPos[1], shipPos[2]);
	dJointSetHingeAxis(chainJoints[LINK_GRASP_CON], 0, 0, 1);

	// Joint between ship and middle link
	dJointAttach( m_oMiddleChainJoint, chainLinks[LINK_GRASP_CON]->GetBody(), this->m_pOwner->GetBody() );  
	dJointSetHingeAnchor(m_oMiddleChainJoint, shipPos[0] , shipPos[1], shipPos[2]);
	dJointSetHingeAxis(m_oMiddleChainJoint, 0, 0, 1);
	
	dJointSetHingeParam( m_oMiddleChainJoint, dParamBounce, 0 ); 
	dJointSetHingeParam( m_oMiddleChainJoint, dParamStopCFM, CFM ); 
	dJointSetHingeParam( m_oMiddleChainJoint, dParamStopERP, ERP ); 
	

}

void CHook::Eject()
{
	if(m_eHookState == CONNECTED){

		m_eHookState = HOMING;
		Vector hookPos = this->GetPosition();

		dJointAttach(m_pLastChainJoint, dJointGetBody(m_pLastChainJoint, 0), m_oPhysicsData.body);
		dJointSetHingeAnchor(m_pLastChainJoint, hookPos[0] , hookPos[1], hookPos[2]);
		dJointSetHingeAxis(m_pLastChainJoint, 0, 0, 1);
		
		dJointSetHingeParam( m_pLastChainJoint, dParamVel, 0 ); 
		dJointSetHingeParam( m_pLastChainJoint, dParamBounce, 0 ); 
		dJointSetHingeParam( m_pLastChainJoint, dParamStopCFM, CFM ); 
		dJointSetHingeParam( m_pLastChainJoint, dParamStopERP, ERP ); 

		m_oPhysicsData.m_bHasCollision = true;
		
		Vector shipFor = m_pOwner->GetForwardVector() * 3000000.0f;

		dBodyAddForce(m_oPhysicsData.body, shipFor[0], shipFor[1], 0.0f);
	}

}

void CHook::Reconnect()
{

}

void CHook::Throw()
{

	//dJointDestroy(m_oHookGrabJoint);
	dJointAttach(m_oHookGrabJoint, 0, 0);

	m_eHookState = RETURNING;
	m_oPhysicsData.m_bAffectedByGravity = false;
	m_oPhysicsData.m_bHasCollision = false;

	m_pGrabbedObject->ToggleIgnore( m_pOwner->GetPhysicsData() );
	m_pGrabbedObject->m_bHasCollision = true;
	m_pGrabbedObject->m_bAffectedByGravity = true;
	dMass mass; 
	dMassSetBox(&mass, 1, 1, 1, 1); 
	dMassAdjust(&mass, m_pGrabbedObject->m_fMass); 
	dBodySetMass(m_pGrabbedObject->body, &mass);
	//Vector(m_pOwner->GetBody()->lvel).CopyInto( m_pGrabbedObject->body->lvel );
	m_pGrabbedObject = NULL;


//	Vector n;
//	n.CopyInto( m_oPhysicsData.body->lvel );
//	n.CopyInto( m_oPhysicsData.body->avel );
	
}

void CHook::AddChainForce(float x_force, float y_force)
{
	frontForce = Vector(x_force, -y_force, 0.0f);
}

void CHook::ApplyForceFront()
{

	if(m_eHookState == GRASPING){

		Vector shipPos = m_pOwner->GetPosition() + (m_pOwner->GetForwardVector() * CENT_DIST);
;
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
	} else if(m_eHookState == GRASPING && !isRadialCorrected){		
		Vector shipPos = m_pOwner->GetPosition();
		Vector diff =  this->GetPosition() - shipPos;
		std::stringstream ss;
		ss << "Length: " << diff.Length();
		CLogManager::Instance()->LogMessage(ss.str());

		if(diff.Length() < (LINK_AMOUNT * 2 - LINK_GRASP_CON) * LINK_LENGTH * 1.5  ){
			CLogManager::Instance()->LogMessage("Radial Correction.");
			
			isRadialCorrected = true;
			diff.Normalize();
			diff *= (LINK_AMOUNT * 2 - LINK_GRASP_CON) * LINK_LENGTH;
			Vector hookPos = diff + shipPos;
			dJointAttach( m_oAngleJoint, m_oPhysicsData.body, this->m_pOwner->GetBody() );
			dJointSetHingeAnchor(m_oAngleJoint, hookPos[0] , hookPos[1], hookPos[2]);
			dJointSetHingeAxis(m_oAngleJoint, 0, 0, 1);
			dJointSetHingeParam( m_oAngleJoint, dParamBounce, 0 ); 
			dJointSetHingeParam( m_oAngleJoint, dParamStopCFM, CFM ); 
			dJointSetHingeParam( m_oAngleJoint, dParamStopERP, ERP ); 
		} else {
			Vector counterForce = diff * -10;
			dBodyAddForce(m_oPhysicsData.body, counterForce[0], counterForce[1], 0.0f);

		}

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
