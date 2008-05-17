#include <string>
#include <sstream>
#include "LogManager.h"

#include "Hook.h"
#include "ChainLink.h"
#include "PlayerObject.h"

#include "ODEManager.h"
#include "ResourceManager.h"
#include "AnimatedTexture.h"

#define LINK_LENGTH 40.0f//10.0f 
#define LINK_AMOUNT 2  //8
#define LINK_GRASP_CON 2//8
#define CENT_DIST -18
#define CFM 0.001f
#define ERP 0.8f

CHook::CHook( CPlayerObject *pOwner )
	: m_pOwner(pOwner), m_eHookState(CONNECTED), m_bIsRadialCorrected(false), m_pGrabbedObject(NULL), m_oHookGrabJoint(NULL), 
	  m_pLastChainJoint(NULL), m_oMiddleChainJoint(NULL), m_oAngleJoint(NULL)
{
	// Game logic settings
	m_eType = HOOK;

	// Render settings
	m_pImage = new CAnimatedTexture("media/scripts/hook.txt");
	SetDepth( -1.1f );

	// Physics settings
	CODEManager* ode = CODEManager::Instance();
	ode->CreatePhysicsData(this, &m_oPhysicsData, 32.0f);
	m_oPhysicsData.ToggleIgnore( pOwner->GetPhysicsData() );
	m_oPhysicsData.m_bAffectedByGravity = false;
	m_oPhysicsData.m_bHasCollision = false;
	m_oPhysicsData.m_fAirDragConst = 0.5f;
	this->SetMass(1.0f);

	// Hook position + orientation
	Vector shipPosition = m_pOwner->GetPosition();
	Vector forward = m_pOwner->GetForwardVector();
	this->SetPosition(shipPosition + forward*CENT_DIST);

	// Joints
	m_pLastChainJoint = ode->createHingeJoint();
	m_oMiddleChainJoint = ode->createHingeJoint();
	m_oHookGrabJoint = ode->createHingeJoint();
	m_oAngleJoint = ode->createHingeJoint();

	// Create chains
	dJointID curJointID;
	CChainLink* curLink;
	dBodyID prevBodyID = m_pOwner->GetBody();
	Vector centerPos = shipPosition + Vector(LINK_LENGTH / 2, 0.0f, 0.0f);
	Vector anchorPoint;

	for(int i = 0; i < LINK_AMOUNT * 2; i++)
	{
		// New chainlink
		curLink = new CChainLink(pOwner);
		chainLinks.push_back(curLink);
		dBodySetPosition( curLink->GetBody(), centerPos[0], centerPos[1], centerPos[2] );

		// Joint with previous link / ship
		curJointID = dJointCreateHinge(ode->getWorld(), 0);
		chainJoints.push_back(curJointID);
		dJointAttach(curJointID, prevBodyID, curLink->GetBody());
		anchorPoint = (i % 2 == 0 ? shipPosition : shipPosition + Vector(LINK_LENGTH, 0.0f, 0.0f));
		dJointSetHingeAnchor(curJointID, anchorPoint[0], anchorPoint[1], anchorPoint[2]);
		dJointSetHingeAxis(curJointID, 0, 0, 1);
		dJointSetHingeParam( curJointID, dParamStopCFM, CFM ); 
		dJointSetHingeParam( curJointID, dParamStopERP, ERP ); 

		prevBodyID = curLink->GetBody();		// Current chain will be joint in the next iteration
	}

	// Atach Ship to last ChainLink
	//m_pLastChainJoint = ode->createHingeJoint();
	chainJoints.push_back(curJointID);
	dJointAttach(m_pLastChainJoint, prevBodyID, this->m_pOwner->GetBody());
	dJointSetHingeAnchor(m_pLastChainJoint, shipPosition[0] , shipPosition[1], shipPosition[2]);

}

CHook::~CHook()
{
	
	for(int i = 0; i < LINK_AMOUNT * 2; i++)
	{
		//dJointAttach(chainJoints[i], 0, 0);
		dJointDestroy(chainJoints[i]);
		delete chainLinks[i];
	}

	if(this->m_pLastChainJoint)
	{
		//dJointAttach(m_pLastChainJoint, 0, 0);
		dJointDestroy(m_pLastChainJoint);
	}	

	if(m_oMiddleChainJoint)
	{
		//dJointAttach(m_oMiddleChainJoint, 0, 0);
		dJointDestroy(m_oMiddleChainJoint);
	}

	if(m_oHookGrabJoint)
	{
		//dJointAttach(m_oHookGrabJoint, 0, 0);
		dJointDestroy(m_oHookGrabJoint);
	}

	if(m_oAngleJoint)
	{
		//dJointAttach(m_oAngleJoint, 0, 0);
		dJointDestroy(m_oAngleJoint);
	}

}

void CHook::SetGrasped(PhysicsData* toGrasp)
{
	m_eHookState = GRASPING;
	m_pGrabbedObject = toGrasp;

}

void CHook::Grasp()
{

	CLogManager::Instance()->LogMessage("Enter grasp" );

	m_eHookState = SWINGING;
	CODEManager* ode = CODEManager::Instance();

	// Clear previous joints on object to grasp, if applicable
	if ( m_pGrabbedObject->m_pOwner->getType() == PLANET && m_pGrabbedObject->planetData->bIsOrbitting)
	{
		//CODEManager::Instance()->DestroyJoint(m_pGrabbedObject->planetData->orbitJoint);
		//ode->DestroyJoint(m_pGrabbedObject->planetData->orbitJoint);
		dJointAttach(m_pGrabbedObject->planetData->orbitJoint, 0, 0);
		m_pGrabbedObject->planetData->bIsOrbitting = false;
	}



	// Create grab joint
	CLogManager::Instance()->LogMessage("Starting to set hookjoint" );
	//m_oHookGrabJoint = CODEManager::Instance()->createHingeJoint();
	dJointAttach( m_oHookGrabJoint, m_oPhysicsData.body, m_pGrabbedObject->body );
	CLogManager::Instance()->LogMessage("Attached hookjoint" );
	m_pGrabbedObject->m_bHasCollision = false;
	m_pGrabbedObject->m_bAffectedByGravity = false;
	m_pGrabbedObject->m_pOwner->SetMass(0.5f, false);	// Remove movement lag

	CLogManager::Instance()->LogMessage("Exit grasp" );

}

void CHook::Eject()
{
	m_eHookState = HOMING;
	m_oPhysicsData.m_bHasCollision = true;

	Vector hookPos = this->GetPosition();
	dJointAttach(m_pLastChainJoint, dJointGetBody(m_pLastChainJoint, 0), m_oPhysicsData.body);
	dJointSetHingeAnchor(m_pLastChainJoint, hookPos[0] , hookPos[1], hookPos[2]);

	// Shoot the hook forward
	Vector shipFor = m_pOwner->GetForwardVector() * 6000000.0f;
	dBodyAddForce(m_oPhysicsData.body, shipFor[0], shipFor[1], 0.0f);
}

void CHook::Retract()
{
	m_eHookState = RETURNING;
}

void CHook::Throw()
{
	CLogManager::Instance()->LogMessage("Enter throw" );

	m_eHookState = RETURNING;

	// Joint between hook and object is destroyed
	dJointAttach(m_oHookGrabJoint, NULL, NULL);
	//dJointDestroy(m_oHookGrabJoint);
	//m_oHookGrabJoint = NULL;

	CLogManager::Instance()->LogMessage("Throwing object" );

	// Joint between hook and ship is destroyed
	if(m_oAngleJoint){
		dJointAttach(m_oAngleJoint, NULL, NULL);
		//dJointDestroy(m_oAngleJoint);
		//m_oAngleJoint = NULL;
	}

	m_oPhysicsData.m_bHasCollision = false;

	// Throwed object gets updated
	m_pGrabbedObject->ToggleIgnore( m_pOwner->GetPhysicsData() );
	m_pGrabbedObject->m_bHasCollision = true;
	m_pGrabbedObject->m_bAffectedByGravity = true;
	m_pGrabbedObject->m_pOwner->ResetMass();
	Vector forward = this->m_pOwner->GetForwardVector();
	Vector shipPos = this->m_pOwner->GetPosition();
	Vector nullVec;
	Vector hookVel = dBodyGetLinearVel( m_pGrabbedObject->body );
	Vector shipVel = dBodyGetLinearVel( this->m_pOwner->GetBody() );
	m_pGrabbedObject->m_pOwner->SetPosition(shipPos + forward * (m_pGrabbedObject->m_fRadius + m_pOwner->GetPhysicsData()->m_fRadius + 5 ) );
	dBodySetLinearVel(m_pGrabbedObject->body, 0.0f, 0.0f, 0.0f);
	
	m_pGrabbedObject->m_pOwner->AddForce(forward * (shipVel.Length() + hookVel.Length()) * 200000);
	m_pGrabbedObject = NULL;

	CLogManager::Instance()->LogMessage("Exit throw" );


}

void CHook::adjustPos(Vector displacement)
{
	this->SetPosition(this->GetPosition() + displacement);

	for(int i = 0; i < LINK_AMOUNT * 2; i++)
	{
		chainLinks[i]->SetPosition(chainLinks[i]->GetPosition() + displacement);
	}
	
}

void CHook::AddChainForce(float x_force, float y_force)
{
	frontForce = Vector(x_force, -y_force, 0.0f);
}

void CHook::ApplyForceFront()
{

	if(m_eHookState == SWINGING && m_bIsRadialCorrected){

		Vector shipPos = m_pOwner->GetPosition() + (m_pOwner->GetForwardVector() * CENT_DIST);

		Vector hookPos = dBodyGetPosition( this->GetBody() );
		Vector radial  = hookPos - shipPos;
		radial.Normalize();
		Vector tangent(radial[1], -radial[0], 0.0f);
		
		if( frontForce[0] * frontForce[1] != 0.0f ){
			float force = frontForce.Length();
			float inprod = tangent.DotProduct( frontForce );
			
			tangent *= force * 50;
			if(tangent.Length() > 15.0f){
				tangent.Normalize();
				tangent *= 15.0f;
			}
		}
		
		dBodyAddForce(m_oPhysicsData.body, tangent[0] * 150.0f, tangent[1] * 150.0f, 0.0f);
	} else if (m_eHookState == HOMING) {


	}
}

void CHook::Update( float fTime )
{

	switch(m_eHookState)
	{
		case CONNECTED:
		{
			m_fAngle = m_pOwner->GetRotation();
			Vector shipFor = m_pOwner->GetForwardVector();
			Vector shipPos = m_pOwner->GetPosition();
			this->SetPosition(shipPos + shipFor * CENT_DIST);
			break;
		}
		case GRASPING:
		{
			this->Grasp();
			break;
		}
		case SWINGING:
		{
			if(!m_bIsRadialCorrected)
			{
				Vector shipPos = m_pOwner->GetPosition();
				Vector hookPos = this->GetPosition();
				Vector diff =  hookPos - shipPos;

				if(diff.Length() < (LINK_AMOUNT * 2 - LINK_GRASP_CON) * LINK_LENGTH + 30)
				{
					m_bIsRadialCorrected = true;

					/*
					// Joint between ship and before-middle link
					chainLinks[LINK_GRASP_CON - 1]->SetPosition(shipPos);
					dJointAttach( chainJoints[LINK_GRASP_CON], chainLinks[LINK_GRASP_CON - 1]->GetBody(), this->m_pOwner->GetBody() );
					dJointSetHingeAnchor(chainJoints[LINK_GRASP_CON], shipPos[0] , shipPos[1], shipPos[2]);

					// Joint between ship and middle link
					chainLinks[LINK_GRASP_CON]->SetPosition(shipPos);
					m_oMiddleChainJoint	= CODEManager::Instance()->createHingeJoint();
					dJointAttach( m_oMiddleChainJoint, chainLinks[LINK_GRASP_CON]->GetBody(), this->m_pOwner->GetBody() );  
					dJointSetHingeAnchor(m_oMiddleChainJoint, shipPos[0] , shipPos[1], shipPos[2]);
*/
					diff.Normalize();
					diff *= (LINK_AMOUNT * 2 - LINK_GRASP_CON) * LINK_LENGTH;
					Vector hookPos = diff + shipPos;
					this->SetPosition(hookPos);
					//m_oAngleJoint = CODEManager::Instance()->createHingeJoint();
					dJointAttach( m_oAngleJoint, m_oPhysicsData.body, this->m_pOwner->GetBody() );
					dJointSetHingeAnchor(m_oAngleJoint, hookPos[0] , hookPos[1], hookPos[2]);
				} 
				else 
				{

					Vector tangent(diff[1], -diff[0], 0.0f);
					tangent.Normalize();
					tangent *= (LINK_AMOUNT * 2 - LINK_GRASP_CON) * LINK_LENGTH;
					Vector destDirection = (tangent + shipPos) - this->GetPosition();
					Vector counterForce = destDirection * 2000;
					//Vector counterForce = (shipPos - hookPos).GetNormalized() * 50.0f;
					dBodyAddForce(m_oPhysicsData.body, counterForce[0], counterForce[1], 0.0f);
				} 

			}
			break;
		}
		case RETURNING:
		{
			Vector shipPos = m_pOwner->GetPosition();
			Vector destPos = shipPos + m_pOwner->GetForwardVector() * CENT_DIST;
			Vector diff = this->GetPosition() - destPos;

			if(diff.Length() > 25.0f){
				Vector change = diff * -10000.0f;
				dBodyAddForce(m_oPhysicsData.body, change[0], change[1], 0.0f);
			} else {
				m_eHookState = CONNECTED;

				// Reattach last link to the ship
				CChainLink* lastLink = chainLinks[LINK_AMOUNT * 2 - 1];
				lastLink->SetPosition(shipPos);
				dJointAttach(m_pLastChainJoint, lastLink->GetBody(), m_pOwner->GetBody());
				dJointSetHingeAnchor(m_pLastChainJoint, shipPos[0] , shipPos[1], 0.0f);
				Vector nullVec;
				m_oPhysicsData.m_pOwner->SetLinVelocity(nullVec);
				m_oPhysicsData.m_pOwner->SetAngVelocity(nullVec);

				if(m_bIsRadialCorrected){

					if ( false && m_oMiddleChainJoint )
					{
						// Detach middle chainlink from ship
						dJointAttach(m_oMiddleChainJoint, NULL, NULL);
						//dJointDestroy(m_oMiddleChainJoint);
						//m_oMiddleChainJoint = NULL;
					}

					chainLinks[LINK_GRASP_CON - 1]->SetPosition( shipPos + Vector(LINK_LENGTH / 2, 0.0f, 0.0f) );
					chainLinks[LINK_GRASP_CON]->SetPosition( shipPos + Vector(LINK_LENGTH / 2, 0.0f, 0.0f) );
					dJointAttach(chainJoints[LINK_GRASP_CON], chainLinks[LINK_GRASP_CON - 1]->GetBody(), chainLinks[LINK_GRASP_CON]->GetBody());
					dJointSetHingeAnchor(chainJoints[LINK_GRASP_CON], shipPos[0], shipPos[1], 0.0f); 
					m_bIsRadialCorrected = false;
				}


			}

			break;
		}
	}

	CBaseMovableObject::Update(fTime);

}

void CHook::SetAlpha( float fAlpha )
{
	m_fAlpha = fAlpha;
	for ( unsigned int i = 0; i<chainLinks.size(); i++ )
	{
		chainLinks[i]->SetAlpha( fAlpha );
	}
}

void CHook::SetInvincibleTime( float fTime )
{
	m_fInvincibleTime = fTime;
	for ( unsigned int i = 0; i<chainLinks.size(); i++ )
	{
		chainLinks[i]->SetInvincibleTime( fTime );
	}
}
