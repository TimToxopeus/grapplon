#include "ODEManager.h"
#include "LogManager.h"
#include "BaseObject.h"
#include "Planet.h"
#include <iostream>
#include <string>
#include <sstream>

#include "ResourceManager.h"
#include "Sound.h"

#include "ode/collision_kernel.h"

#pragma warning(disable:4018)

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
	m_oSpace = dHashSpaceCreate(0);	


	dWorldSetCFM(m_oWorld, 0.8f);

	m_oContactgroup = dJointGroupCreate(MAX_CONTACTS);
	m_oJointgroup = dJointGroupCreate(MAX_HINGES);

	//dWorldSetGravity (m_oWorld, 0, 9.81, 0);

}

CODEManager::~CODEManager()
{
	CLogManager::Instance()->LogMessage("Terminating ODE manager.");

	dJointGroupDestroy(m_oJointgroup);
	dJointGroupDestroy( m_oContactgroup );

	CLogManager::Instance()->LogMessage("Cleanin' up da bodies..");
	for ( int i = 0; i<m_vBodies.size(); i++ )
	{
		if(m_vBodies[i]->geom)	// TODO: Niet alle PhysicsData hebben een Geom (e.g. Chainlink)
//			dGeomDestroy( m_vBodies[i]->geom );
		dBodyDestroy( m_vBodies[i]->body );
	}
	m_vBodies.clear();

	dWorldDestroy( m_oWorld );
	dSpaceDestroy( m_oSpace );
}

void CODEManager::Update( float fTime )
{
	std::stringstream ss;
	float nbSecondsByStep = 0.001f; 

	// Find the corresponding number of steps that must be taken 
	int nbStepsToPerform = static_cast<int>(fTime/nbSecondsByStep); 
	CLogManager::Instance()->LogMessage("ODE performing " + itoa2(nbStepsToPerform) + " steps" );

/*
	if ( nbStepsToPerform > 50 )
	{
		nbStepsToPerform = 50;
		nbSecondsByStep = fTime / nbStepsToPerform;
		CLogManager::Instance()->LogMessage("ODE step count clamped to 50." );
	}
*/
	// Make these steps to advance world time 
	for (int i = 0; i < nbStepsToPerform; i++) 
	{
		ApplyGravity();
		ApplyMotorForceAndDrag();

		m_iContacts = 0;
		dSpaceCollide( m_oSpace, 0, collideCallback );
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

dGeomID CODEManager::CreateGeom( dBodyID body, float fLength, float fThick )
{
	dGeomID geom = dCreateBox( m_oSpace, fLength, fThick, 10.0f);
	dGeomSetBody(geom, body);
	return geom;
}


void CODEManager::CreatePhysicsDataBox( CBaseObject *pOwner, PhysicsData &d, float fLength, float fThick )
{
	if ( d.geom )
		dGeomDestroy(d.geom);
	if ( d.body )
		dBodyDestroy(d.body);

	d.m_pOwner = pOwner;
	d.body = CreateBody();
	//d.geom = CreateGeom( d.body, fLength, fThick );
	d.m_fGravConst = 0.0f;
	d.m_bAffectedByGravity = true;
	d.m_bHasCollision = true;
	d.m_bIsHook = false;
	d.m_oHookGrabJoint = 0;
	d.body->userdata = &d;
	d.m_pGrabbedObject = 0;
	d.m_bIsPlanet = false;

	AddData( &d );

}

void CODEManager::CreatePhysicsData( CBaseObject *pOwner, PhysicsData &d, float fRadius )
{
	if ( d.geom )
		dGeomDestroy(d.geom);
	if ( d.body )
		dBodyDestroy(d.body);

	d.m_pOwner = pOwner;
	d.body = CreateBody();
	d.geom = CreateGeom( d.body, fRadius );
	d.m_fGravConst = 0.0f;
	d.m_bAffectedByGravity = true;
	d.m_bHasCollision = true;
	d.m_bIsHook = false;
	d.m_oHookGrabJoint = 0;
	d.body->userdata = &d;
	d.m_pGrabbedObject = 0;
	d.m_bIsPlanet = false;

	AddData( &d );
}

void CODEManager::CollisionCallback(void *pData, dGeomID o1, dGeomID o2)
{
	return;

	PhysicsData *d1 = GetPhysicsDataByGeom(o1);
	PhysicsData *d2 = GetPhysicsDataByGeom(o2);

	if (!d1 || !d2) return;

	if ( dGeomIsSpace(o1) || dGeomIsSpace(o2) )
	{
		// colliding a space with something :
		dSpaceCollide2( o1, o2, pData, &collideCallback );

		// collide all geoms internal to the space(s)
		if ( dGeomIsSpace(o1) )
			dSpaceCollide( (dSpaceID)o1, pData, &collideCallback );
		if ( dGeomIsSpace(o2) )
			dSpaceCollide( (dSpaceID)o2, pData, &collideCallback );
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
	std::vector<PhysicsData *>::iterator itO;
	
	Vector posO;

	PhysicsData object;

	Vector force;
	Vector newForce;

	for(itO = m_vBodies.begin(); itO != m_vBodies.end(); itO++)
	{

		object = *(*itO);
		
		if(object.m_fAirDragConst == 0.0f) continue;

		object.m_pOwner->ApplyForceFront();		

		if ( object.m_fGravConst != 0.0f ) continue;		// Skip planets

		force = dBodyGetLinearVel(object.body);
		
		newForce = force * -object.m_fAirDragConst;//Vector(-5.0f * force[0], -5.0f * force[1], -5.0f * force[2]);

		dBodyAddForce(object.body, newForce[0], newForce[1], 0.0f);

	}

}

void CODEManager::ApplyGravity()
{
	std::vector<PhysicsData *>::iterator itP;
	std::vector<PhysicsData *>::iterator itO;
	
	Vector posP;
	Vector posO;

	PhysicsData planet;
	PhysicsData object;

	Vector force;
	float  distance;
	float  forceMag;

	for(itP = m_vBodies.begin(); itP != m_vBodies.end(); itP++)
	{
		planet = *(*itP);
		posP   = Vector(planet.body->posr.pos);
		if ( planet.m_fGravConst == 0.0f ) continue;

		for(itO = m_vBodies.begin(); itO != m_vBodies.end(); itO++)
		{
			object = *(*itO);
			posO   = Vector(object.body->posr.pos);

			if( object == planet ) continue;
			if ( !object.m_bAffectedByGravity ) continue;

			// Vector Object --> Planeet
			force = posP - posO;

			//ss << "x: " << force[0] << " y: " << force[1] << " z: " << force[2];

			// Distance between Object and Planet
			distance = force.Length();
			
			//ss << " distance: " << length;

			if ( abs(distance) >= 0.00001f )
			{
				// Normalize
				force.Normalize();

				//ss << " m1: " << (*itP)->GetMass() << " m2: " << (*itO)->GetMass();

				forceMag = (planet.m_fGravConst * planet.body->mass.mass * object.body->mass.mass ) / (distance * distance);
				force *= forceMag;

				//ss << " f_na_x: " << force[0] << " f_na_y: " << force[1] << " f_na_z: " << force[2];

				dBodyAddForce(object.body, force[0], force[1], 0.0f);
			}
			//CLogManager::Instance()->LogMessage(ss.str());
		}
	}
}

void CODEManager::HandleCollisions()
{
	return;

	for ( int i = 0; i<m_iContacts; i++ )
	{
		dContactGeom c = m_oContacts[i];

		int collisionMode = 1;
		bool sound = true;

		if ( collisionMode == 1 )
		{
			dContact contact;
			contact.geom = c;
			PhysicsData *d = (PhysicsData *)c.g1->body->userdata;
			PhysicsData *d2 = (PhysicsData *)c.g2->body->userdata;

			if(!d || !d2) continue;

			Vector force = Vector( c.normal ) * c.depth;

			d->m_pOwner->CollideWith( d2->m_pOwner, force );
			d2->m_pOwner->CollideWith( d->m_pOwner, force );

			// Check if it's a hook
			if ( !d->m_bIsHook )
			{
				// This is a collision between two non-hook objects
				contact.surface.mode = dContactBounce | dContactSoftCFM;
				contact.surface.mu = dInfinity;
				contact.surface.mu2 = 0;
				contact.surface.bounce = 1;
				contact.surface.bounce_vel = 0;
				contact.surface.soft_cfm = 1e-6f; 

				dJointID joint = dJointCreateContact(m_oWorld, m_oContactgroup, &contact);
				dJointAttach(joint, c.g1->body, c.g2->body);
			}
			else
			{
				// This is a collision between a hook and another object. Check if the hook doesn't already have something grabbed
				if ( d->m_oHookGrabJoint == 0 )
				{
					// Nope, we're home-free to grab

					// Clear previous joints if applicable
					if ( d2->m_bIsPlanet )
					{
						if ( d2->planetData->orbitJoint )
						{
							dJointAttach( d2->planetData->orbitJoint, 0, 0 );
							dJointDestroy( d2->planetData->orbitJoint );
							d2->planetData->orbitJoint = NULL;
						}
					}

					// Create grab joint
					dJointID joint = dJointCreateHinge(m_oWorld, 0);
					dJointAttach( joint, c.g1->body, c.g2->body );
					d->m_oHookGrabJoint = joint;
					d->m_pGrabbedObject = (PhysicsData *)c.g2->body->userdata;
					d->m_pGrabbedObject->m_bHasCollision = false;
					d->m_pGrabbedObject->m_bAffectedByGravity = false;

					// Set the mass to 1 to remove movement lag
					dMass mass; 
					dMassSetBox(&mass, 1, 1, 1, 1); 
					dMassAdjust(&mass, 1); 
					dBodySetMass(d->m_pGrabbedObject->body, &mass);
				}
			}
		}
		else
		{
			Vector normal = Vector(c.normal);

			dBodyID b1 = dGeomGetBody(c.g1);
			dBodyID b2 = dGeomGetBody(c.g2);

			Vector v1 = Vector(b1->lvel);
			Vector v2 = Vector(b2->lvel);

			Vector v1New = v1.Mirror( normal ).GetNormalized();
			Vector v2New = v2.Mirror( normal ).GetNormalized();

			float speed = v1.Length() + v2.Length();
			float speed_per_mass = speed / (b1->mass.mass + b2->mass.mass);

			v1New *= (b1->mass.mass * speed_per_mass) * 1.1f;
			v2New *= (b2->mass.mass * speed_per_mass) * 1.1f;

			v1New.CopyInto( b1->lvel );
			v2New.CopyInto( b2->lvel );
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

PhysicsData *CODEManager::GetPhysicsDataByGeom( dGeomID o )
{
	for ( int i = 0; i<m_vBodies.size(); i++ )
	{
		PhysicsData *d = m_vBodies[i];
		if ( d->geom == o )
			return d;
	}
	return NULL;
}

void CODEManager::AddData( PhysicsData *pData )
{
	for ( int i = 0; i < m_vBodies.size(); i++ )
	{
		if ( m_vBodies[i] == pData )
			return;
	}
	m_vBodies.push_back(pData);
}

dJointID CODEManager::CreateJoint( dBodyID b1, dBodyID b2, float x, float y )
{
	dJointID joint = dJointCreateBall( m_oWorld, m_oJointgroup );
	dJointAttach( joint, b1, b2 );
	dJointSetBallAnchor( joint, x, y, 0 );
	m_vJoints.push_back( joint );
	return joint;
}

void CODEManager::DestroyJoint( dJointID joint )
{
	dJointAttach( joint, 0, 0 );
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
