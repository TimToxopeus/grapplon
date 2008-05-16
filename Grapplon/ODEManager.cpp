#include "ODEManager.h"
#include "LogManager.h"
#include "BaseObject.h"
#include "Planet.h"
#include <iostream>
#include <string>
#include <sstream>
#include "Hook.h"

#include "ResourceManager.h"
#include "Sound.h"

#include "ode/collision_kernel.h"

#include "Vector.h"

#define CFM 0.001f
#define ERP 0.8f


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
//	m_oSpace = dSimpleSpaceCreate(0);
	m_oSpace = dHashSpaceCreate(0);
//	m_oSpace = dQuadTreeSpaceCreate( 0, centerv, extentsv3, 4 );


	//dWorldSetCFM(m_oWorld, 0.8f);

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
		if(m_vPlanets[i]->geom != NULL)	// TODO: Niet alle PhysicsData hebben een Geom (e.g. Chainlink)
			dGeomDestroy( m_vPlanets[i]->geom );
		dBodyDestroy( m_vPlanets[i]->body );
	}
	m_vPlanets.clear();


	for ( unsigned int i = 0; i<m_vOthers.size(); i++ )
	{
		if(m_vOthers[i]->geom != NULL)	// TODO: Niet alle PhysicsData hebben een Geom (e.g. Chainlink)
			dGeomDestroy( m_vOthers[i]->geom );
		dBodyDestroy( m_vOthers[i]->body );
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
		ApplyGravity();
		ApplyMotorForceAndDrag();

		m_iContacts = 0;
		dSpaceCollide( m_oSpace, 0, collideCallback );
		//m_oSpace->collide( 0, collideCallback );
		HandleCollisions();

		// Step world
		dWorldQuickStep(m_oWorld, nbSecondsByStep); 
		//dWorldStepFast1(m_oWorld, nbSecondsByStep / 10.0f, nbStepsToPerform * 10.0f); 
		
		// Remove all temporary collision joints now that the world has been stepped 
		dJointGroupEmpty(m_oContactgroup);   
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
	d->m_bHasCollision = hasGeom;
	dBodySetData(d->body, d);

	if(hasGeom)
	{
		d->geom = CreateGeom( d->body, fRadius );
		AddData( d );
	}
	else
	{
		d->geom = NULL;
	}
	

}

void CODEManager::CollisionCallback(void *pData, dGeomID o1, dGeomID o2)
{

	PhysicsData *d1 = (PhysicsData *)dBodyGetData( dGeomGetBody(o1) );
	PhysicsData *d2 = (PhysicsData *)dBodyGetData( dGeomGetBody(o2) );

	if (!d1 || !d2) return;


	if ( dGeomIsSpace(o1) || dGeomIsSpace(o2) )
	{
		// colliding a space with something :
		dSpaceCollide2( o1, o2, pData, &collideCallback );

		// collide all geoms internal to the space(s)
		if ( dGeomIsSpace(o1) )
			dSpaceCollide( (dSpaceID)o1, pData, &collideCallback );
			//(dSpaceCollide( (dSpaceID) o1, pData, &collideCallback );
		if ( dGeomIsSpace(o2) )
			dSpaceCollide( (dSpaceID)o2, pData, &collideCallback );
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
//	std::vector<PhysicsData *>::iterator itO;
	PhysicsData* curObject;
	Vector airDragForce;

	unsigned int i = 0;
	for ( i; i<m_vOthers.size(); i++ )
	{
		curObject = m_vOthers[i];

		//if( curObject->m_pOwner->getType() == PLANET) continue;	// Skip planets
		
		if(curObject->m_fAirDragConst != 0.0f){
			airDragForce = Vector(dBodyGetLinearVel(curObject->body)) * -curObject->m_fAirDragConst;
			dBodyAddForce(curObject->body, airDragForce[0], airDragForce[1], 0.0f);
		}

		curObject->m_pOwner->ApplyForceFront();		

	}

}

void CODEManager::ApplyGravity()
{
//	std::vector<PhysicsData*>::iterator itP;
//	std::vector<PhysicsData*>::iterator itO;
	
	Vector posP;
	Vector posO;

	PhysicsData* planet;
	PhysicsData* object;

	Vector force;
	float  distance;
	float  forceMag;

	unsigned int i, j;
	i = j = 0;

	for ( i; i<m_vPlanets.size(); i++ )
	{
		planet = m_vPlanets[i];

		posP = planet->m_pOwner->GetPosition();

		for ( j; j < m_vOthers.size(); j++ )
		{
			object = m_vOthers[j];
			if ( !object->m_bAffectedByGravity ) continue;

			posO = object->m_pOwner->GetPosition();

			// Vector Object --> Planeet
			force = posP - posO;

			// Distance between Object and Planet
			distance = force.Length();
			if ( distance > 300 + planet->m_fRadius){
				continue;
			}

			if ( distance >= 0.00001f )
			{
				// Normalize
				force.Normalize();

				forceMag = (planet->m_fGravConst * planet->m_pOwner->GetMass() * object->m_pOwner->GetMass() ) / (distance * distance);
				force *= forceMag;

				dBodyAddForce(object->body, force[0], force[1], 0.0f);
			}
		}
	}
}

void CODEManager::HandleCollisions()
{
	for ( int i = 0; i<m_iContacts; i++ )
	{
		dContactGeom* c = &m_oContacts[i];

		int collisionMode = 1;
		bool sound = false;

		if ( collisionMode == 1 )
		{
			dContact contact;
			contact.geom = *c;
			PhysicsData *d1 = (PhysicsData *)c->g1->body->userdata;
			PhysicsData *d2 = (PhysicsData *)c->g2->body->userdata;

			if(!d1 || !d2) continue;

			Vector force = Vector( d1->body->lvel ) + Vector( d2->body->lvel ) * -1;

			d1->m_pOwner->CollideWith( d2->m_pOwner, force );
			d2->m_pOwner->CollideWith( d1->m_pOwner, force );

			// Check if it's a hook
			if ( !( (d1->m_pOwner->getType() == HOOK) ^ (d2->m_pOwner->getType() == HOOK) ) )
			{
				// This is a collision between two non-hook objects
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
			{
				bool d1IsHook = d1->m_pOwner->getType() == HOOK;
				CHook* hook = dynamic_cast<CHook*>( (d1IsHook ? d1 : d2)->m_pOwner );
				PhysicsData* grabbee = (d1IsHook ? d2 : d1);

				// This is a collision between a hook and another object. Check if the hook doesn't already have something grabbed
				if ( hook->m_eHookState == HOMING && grabbee->m_fGravConst == 0.0f )
					hook->SetGrasped(grabbee); // Nope, we're home-free to grab

			}
		}
		else
		{
			//Vector normal = Vector(c->normal);

			//dBodyID b1 = dGeomGetBody(c->g1);
			//dBodyID b2 = dGeomGetBody(c->g2);

			//Vector v1 = Vector(b1->lvel);
			//Vector v2 = Vector(b2->lvel);

			//Vector v1New = v1.Mirror( normal ).GetNormalized();
			//Vector v2New = v2.Mirror( normal ).GetNormalized();

			//float speed = v1.Length() + v2.Length();
			//float speed_per_mass = speed / (b1->mass.mass + b2->mass.mass);

			//v1New *= (b1->mass.mass * speed_per_mass) * 1.1f;
			//v2New *= (b2->mass.mass * speed_per_mass) * 1.1f;

			//v1New.CopyInto( b1->lvel );
			//v2New.CopyInto( b2->lvel );
		}

		if ( sound )
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
		}
	}

}

void CODEManager::AddData( PhysicsData *pData )
{

	std::vector<PhysicsData*>* list;

	list = (pData->m_fGravConst == 0.0f ? &m_vOthers : &m_vPlanets);

	
	for ( unsigned int i = 0; i < (*list).size(); i++ )
	{
		if ( (*list)[i] == pData )
			return;
	}

	(*list).push_back(pData);

}


dJointID CODEManager::createHingeJoint(){
	dJointID joint = dJointCreateHinge(m_oWorld, 0);
	dJointSetHingeAxis(joint, 0, 0, 1);
	dJointSetHingeParam(joint, dParamStopCFM, CFM ); 
	dJointSetHingeParam(joint, dParamStopERP, ERP ); 
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
	
	//if(!joint) return;

	//dJointAttach( joint, 0, 0 );
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
