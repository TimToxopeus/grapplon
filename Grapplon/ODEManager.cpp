#include "ODEManager.h"
#include "LogManager.h"
#include "BaseObject.h"
#include "Asteroid.h"
#include <iostream>
#include <string>
#include <sstream>
#include "Hook.h"
#include "Universe.h"
#include "PowerUp.h"

#include "ResourceManager.h"
#include "Sound.h"
#include "GameSettings.h"

#include "ode/collision_kernel.h"

#include "Vector.h"

CODEManager *CODEManager::m_pInstance = NULL;



void collideCallback (void *data, dGeomID o1, dGeomID o2)
{
	CODEManager::Instance()->CollisionCallback( data, o1, o2 );
}

CODEManager::CODEManager()
{
	CLogManager::Instance()->LogMessage("Initializing ODE manager.");
	
	// Create world and space
	m_oWorld = dWorldCreate();

	dVector3 extentsv3, centerv;
	extentsv3[0] = extentsv3[1] = extentsv3[2] = 1000;
	centerv[0] = centerv[1] = centerv[2] = 0;
	m_oSpace = dHashSpaceCreate(0);

	m_oContactgroup = dJointGroupCreate(MAX_CONTACTS);
	m_oJointgroup = dJointGroupCreate(MAX_HINGES);

}

CODEManager::~CODEManager()
{
	CLogManager::Instance()->LogMessage("Terminating ODE manager.");

	dJointGroupDestroy(m_oJointgroup);
	dJointGroupDestroy( m_oContactgroup );

	CLogManager::Instance()->LogMessage("Cleanin' up da bodies..");


	for ( unsigned int i = 0; i<m_vPlanets.size(); i++ )
	{
		dGeomDestroy( m_vPlanets[i]->geom );
		dBodyDestroy( m_vPlanets[i]->body );
	}
	m_vPlanets.clear();

	for ( unsigned int i = 0; i<m_vPlayers.size(); i++ )
	{
		dGeomDestroy( m_vPlayers[i]->geom );
		dBodyDestroy( m_vPlayers[i]->body );
	}
	m_vPlayers.clear();

	for ( unsigned int i = 0; i<m_vAsteroids.size(); i++ )
	{
		dGeomDestroy( m_vAsteroids[i]->geom );
		dBodyDestroy( m_vAsteroids[i]->body );
	}
	m_vAsteroids.clear();

	for ( unsigned int i = 0; i<m_vOthers.size(); i++ )
	{
		//if(m_vOthers[i]->geom != NULL) dGeomDestroy( m_vOthers[i]->geom );		// Chainlinks have no geom
		//if(m_vOthers[i]->body != NULL) dBodyDestroy( m_vOthers[i]->body );
	}
	m_vOthers.clear();

	dWorldDestroy( m_oWorld );
	dSpaceDestroy( m_oSpace );
}

void CODEManager::Update( float fTime )
{
	float nbSecondsByStep = 0.0005f;

	// Find the corresponding number of steps that must be taken 
	int nbStepsToPerform = static_cast<int>(fTime/nbSecondsByStep); 
	
	// Make these steps to advance world time 
	for (int i = 0; i < nbStepsToPerform; i++) 
	{
		ApplyGravity(nbSecondsByStep);
		ApplyMotorForceAndDrag();

		m_iContacts = 0;
		//dSpaceCollide( m_oSpace, 0, collideCallback );
		m_oSpace->collide( 0, collideCallback );
		HandleCollisions();

		// Step world
		dWorldQuickStep(m_oWorld, nbSecondsByStep); 
		//dWorldStepFast1(m_oWorld, nbSecondsByStep / 10.0f, nbStepsToPerform * 10.0f); 
		
		// Remove all temporary collision joints now that the world has been stepped 
		dJointGroupEmpty(m_oContactgroup);
	}

	unsigned int time = SDL_GetTicks();
	unsigned int i = m_vCollisions.size();
	if ( i > 0 )
	{
		i--;
		for ( i; i>0; i-- )
		{
			if ( m_vCollisions[i].time + SOUNDTIME < time )
				m_vCollisions.erase( m_vCollisions.begin() + i );
		}
	}
} 

dBodyID CODEManager::CreateBody()
{
	dBodyID body = dBodyCreate(m_oWorld);
	return body;
}

dGeomID CODEManager::CreateGeom( dBodyID body, float fRadius )
{
	dGeomID geom = dCreateSphere(m_oSpace, fRadius); 
	dGeomSetBody(geom, body);
	return geom;
}

void CODEManager::CreatePhysicsData( CBaseObject *pOwner, PhysicsData* d, float fRadius)
{
	if ( d->geom )
		dGeomDestroy(d->geom);
	if ( d->body )
		dBodyDestroy(d->body);

	bool hasGeom = (fRadius != 0.0f);

	d->m_pOwner = pOwner;
	d->body = CreateBody();

	d->m_fGravConst = 0.0f;
	d->m_fRadius = fRadius;
	d->m_bAffectedByGravity = hasGeom;
	d->m_bAffectedByTemperature = false;
	d->m_bHasCollision = hasGeom;
	dBodySetData(d->body, d);

	if(hasGeom)
	{
		d->geom = CreateGeom( d->body, fRadius );
	}
	else
	{
		d->geom = NULL;
	}
	
		AddData( d );

}

void CODEManager::CollisionCallback(void *pData, dGeomID o1, dGeomID o2)
{

	PhysicsData *d1 = (PhysicsData *) o1->body->userdata;
	PhysicsData *d2 = (PhysicsData *) o2->body->userdata;

	if (!d1 || !d2) return;


	if ( dGeomIsSpace(o1) || dGeomIsSpace(o2) )
	{

		exit(0);
		// colliding a space with something :
	//	dSpaceCollide2( o1, o2, pData, &collideCallback );

		// collide all geoms internal to the space(s)
	//	if ( dGeomIsSpace(o1) )
	//		dSpaceCollide( (dSpaceID)o1, pData, &collideCallback );
			//(dSpaceCollide( (dSpaceID) o1, pData, &collideCallback );
	//	if ( dGeomIsSpace(o2) )
	//		dSpaceCollide( (dSpaceID)o2, pData, &collideCallback );
			//((dSpaceID)o1)->collide( pData, &collideCallback );
	}
	else
	{
		// colliding two non-space geoms, so generate contact
		// points between o1 and o2
		if ( d1->m_bHasCollision && d2->m_bHasCollision )
			if ( d1->CollidesWith( d2 ) && d2->CollidesWith( d1 ) )
				m_iContacts += dCollide( o1, o2, MAX_CONTACTS - m_iContacts, m_oContacts + m_iContacts, sizeof(dContactGeom) );
		// add these contact points to the simulation ...
	}

}


void CODEManager::ApplyMotorForceAndDrag()
{
	PhysicsData* curObject;
	Vector airDragForce;
	Vector pos;
	std::vector<PhysicsData*>* lists[3] = { &m_vOthers, &m_vPlayers, &m_vAsteroids };

	bool correctWidth;
	bool correctHeight;


	for(int il = 0; il < 3; il++)
	{
		for (unsigned int i = 0; i<lists[il]->size(); i++ )
		{
			curObject = (*lists[il])[i];

			ObjectType objType = curObject->m_pOwner->getType();
			if(objType != SHIP && objType != HOOK && objType != ASTEROID) continue;
			if(curObject->m_fAirDragConst != 0.0f){
				airDragForce = Vector(dBodyGetLinearVel(curObject->body)) * -curObject->m_fAirDragConst;
				dBodyAddForce(curObject->body, airDragForce[0], airDragForce[1], 0.0f);
			}

			curObject->m_pOwner->ApplyForceFront();
			
			pos = curObject->m_pOwner->GetPosition();
			
			correctWidth  = abs(pos[0]) > m_pUniverse->m_fWidth;
			correctHeight = abs(pos[1]) > m_pUniverse->m_fHeight;

			if(objType != HOOK && ( correctWidth || correctHeight) )
			{
				if(objType == ASTEROID)
				{
					CAsteroid* asteroid = dynamic_cast<CAsteroid*>(curObject->m_pOwner);
					
					if(!asteroid->m_bIsGrabable) continue;			// Already leaving the field OR Grabbed
					
					if(asteroid->m_fBounceToggleTime < 0.0001f){
						if(asteroid->m_iWallBounces == SETS->WALL_BOUNCES + 1)				// Asteroid exceded maximum bounces, so respawn
						{
							asteroid->LeaveField();
							continue;			// Don't rebounce
						}
						else{
							asteroid->m_iWallBounces++;
							asteroid->m_fBounceToggleTime = SETS->W_BOUNCE_TOGGLE_TIME;
						}
					}
				}

				airDragForce = Vector(0, 0, 0);
				if(correctWidth ) airDragForce[0] = (float) (pos[0] < 0 ? -1 : 1) * -m_pUniverse->m_fBoundaryForce;	
				if(correctHeight) airDragForce[1] = (float) (pos[1] < 0 ? -1 : 1) * -m_pUniverse->m_fBoundaryForce;	
				
				dBodyAddForce(curObject->body, airDragForce[0], airDragForce[1], 0.0f);

			}

		}
	}


}

void CODEManager::ApplyGravity(float timePassed)
{
	Vector posP;
	Vector posO;

	PhysicsData* planetPhys;
	CPlanet* planet;
	PhysicsData* object;

	Vector force;
	float  distance;
	float  forceMag;


	std::vector<PhysicsData*>* lists[2] = { &m_vPlayers, &m_vAsteroids };

	for ( unsigned int i = 0; i<m_vPlanets.size(); i++ )
	{
		planetPhys = m_vPlanets[i];
		planet = dynamic_cast<CPlanet*>(planetPhys->m_pOwner);

		posP = planet->GetPosition();


		for(int il = 0; il < 2; il++)
		{
			for (unsigned int i = 0; i<lists[il]->size(); i++ )
			{
				object = (*lists[il])[i];
				if ( !object->m_bAffectedByGravity && !object->m_bAffectedByTemperature) continue;

				posO = object->m_pOwner->GetPosition();
				force = posP - posO;
				distance = force.Length();

				if(object->m_bAffectedByTemperature && planet->m_iTempRadius > distance)
				{
					if(planet->getType() == SUN)
						object->m_pOwner->IncreaseTemp(timePassed);
					else if(planet->getType() == ICE)
						object->m_pOwner->IncreaseTemp(-timePassed);
				}

				if( !object->m_bAffectedByGravity) continue;

				// Normalize
				force.Normalize();

				forceMag = (planetPhys->m_fGravConst * planet->GetMass() * object->m_pOwner->GetMass() ) / (distance * distance);
				force *= forceMag;

				dBodyAddForce(object->body, force[0], force[1], 0.0f);
			}
		}
	}
}

void CODEManager::HandleCollisions()
{
	unsigned int time = SDL_GetTicks();
	for ( int i = 0; i<m_iContacts; i++ )
	{
		dContactGeom* c = &m_oContacts[i];

		bool sound = true;

		dContact contact;
		contact.geom = *c;
		PhysicsData *d1 = (PhysicsData *)c->g1->body->userdata;
		PhysicsData *d2 = (PhysicsData *)c->g2->body->userdata;

		if(!d1 || !d2) continue;

		Vector force = Vector( d1->body->lvel ) + Vector( d2->body->lvel ) * -1;

		d1->m_pOwner->CollideWith( d2->m_pOwner);
		d2->m_pOwner->CollideWith( d1->m_pOwner);

		
		if ( !( (d1->m_pOwner->getType() == HOOK) ^ (d2->m_pOwner->getType() == HOOK) ) )
		{	// This is a collision between two non-hook objects
			contact.surface.mode = dContactBounce | dContactSoftCFM;
			contact.surface.mu = dInfinity;
			contact.surface.mu2 = 0;
			contact.surface.bounce = 1;
			contact.surface.bounce_vel = 0;
			contact.surface.soft_cfm = 1e-6f; 

			dJointID joint = dJointCreateContact(m_oWorld, m_oContactgroup, &contact);
			dJointAttach(joint, c->g1->body, c->g2->body);
		}
		else	
		{	// Collision between hook and object
			bool d1IsHook = d1->m_pOwner->getType() == HOOK;
			CHook* hook = dynamic_cast<CHook*>( (d1IsHook ? d1 : d2)->m_pOwner );
			PhysicsData* grabbee = (d1IsHook ? d2 : d1);
			
			if(grabbee->m_pOwner->getType() == ASTEROID)
			{
				CAsteroid* asteroid = dynamic_cast<CAsteroid*>(grabbee->m_pOwner);
				if ( hook->m_eHookState == HOMING && asteroid->m_bIsGrabable )		// Nothing grabbed yet && not a planet or ship
					hook->SetGrasped(grabbee);
			}
			else if (grabbee->m_pOwner->getType() == POWERUP)
			{
				CPowerUp* powerup = dynamic_cast<CPowerUp*>(grabbee->m_pOwner);
				if ( hook->m_eHookState == HOMING && powerup->m_bIsGrabable )		// Nothing grabbed yet && not a planet or ship
					hook->SetGrasped(grabbee);
			}
		}

		if ( sound && !HasRecentlyCollided(d1->body, d2->body, time) )
		{
			int r = rand()%4;
			CSound *pSound;
			if ( r == 0 )
				pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/ship_collide1.wav", RT_SOUND);
			if ( r == 1 )
				pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/ship_collide2.wav", RT_SOUND);
			if ( r == 2 )
				pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/ship_collide3.wav", RT_SOUND);
			if ( r == 3 )
				pSound = (CSound *)CResourceManager::Instance()->GetResource("media/sounds/ship_collide4.wav", RT_SOUND);
			pSound->Play();

			Collide collide;
			collide.b1 = d1->body;
			collide.b2 = d2->body;
			collide.time = time;
			m_vCollisions.push_back( collide );
		}
	}

}

void CODEManager::AddData( PhysicsData *pData )
{

	ObjectType oType = pData->m_pOwner->getType();

	std::vector<PhysicsData*>* list = NULL;

	if(oType == SHIP )															list = &m_vPlayers;
	if(oType == ORDINARY || oType == ICE || oType == BROKEN || oType == SUN)	list = &m_vPlanets;
	if(oType == ASTEROID)														list = &m_vAsteroids;
	if(oType == POWERUP)														list = &m_vPowerUps;
	if(list == NULL)															list = &m_vOthers;

	for ( unsigned int i = 0; i < (*list).size(); i++ )
	{
		if ( (*list)[i] == pData )
		{
			return;
		}
	}

	(*list).push_back(pData);
}


dJointID CODEManager::createHingeJoint(char* name)
{
	dJointID joint = dJointCreateHinge(m_oWorld, 0);
	dJointSetData(joint, name);
	dJointSetHingeAxis(joint, 0, 0, 1);
	dJointSetHingeParam(joint, dParamStopCFM, SETS->CFM ); 
	dJointSetHingeParam(joint, dParamStopERP, SETS->ERP ); 
	return joint;
}

dJointID CODEManager::CreateJoint( dBodyID b1, dBodyID b2, float x, float y )
{
	dJointID joint = dJointCreateBall( m_oWorld, 0 );
	dJointAttach( joint, b1, b2 );
	dJointSetBallAnchor( joint, x, y, 0 );
	m_vJoints.push_back( joint );
	return joint;
}

void CODEManager::DestroyJoint( dJointID joint )
{
	for ( unsigned int i = 0; i<m_vJoints.size(); i++ )
	{
		if ( m_vJoints[i] == joint )
		{
			m_vJoints.erase( m_vJoints.begin() + i );
			break;
		}
	}

	dJointDestroy( joint );
}

bool CODEManager::HasRecentlyCollided( dBodyID b1, dBodyID b2, unsigned int curTime )
{
	for ( unsigned int i = 0; i<m_vCollisions.size(); i++ )
	{
		if ( (m_vCollisions[i].b1 == b1 && m_vCollisions[i].b2 == b2) ||
			(m_vCollisions[i].b1 == b2 && m_vCollisions[i].b2 == b1) )
		{
			if ( curTime > m_vCollisions[i].time + SOUNDTIME )
				return true;
		}
	}
	return false;
}
