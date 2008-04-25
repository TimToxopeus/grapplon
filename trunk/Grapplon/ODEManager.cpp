#include "ODEManager.h"
#include "LogManager.h"
#include "BaseObject.h"
#include "Planet.h"
#include <iostream>
#include <string>
#include <sstream>

#include "ResourceManager.h"
#include "Sound.h"

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

	m_oContactgroup = dJointGroupCreate(0);

	//dWorldSetGravity (m_oWorld, 0, 9.81, 0);

}

CODEManager::~CODEManager()
{
	CLogManager::Instance()->LogMessage("Terminating ODE manager.");

	CLogManager::Instance()->LogMessage("Cleanin' up da bodies..");
	for ( int i = 0; i<m_vBodies.size(); i++ )
	{
		dGeomDestroy( m_vBodies[i]->geom );
		dBodyDestroy( m_vBodies[i]->body );
	}
	m_vBodies.clear();

	dJointGroupDestroy( m_oContactgroup );
	dWorldDestroy( m_oWorld );
	dSpaceDestroy( m_oSpace );
}

void CODEManager::Update( float fTime )
{
	std::stringstream ss;
	static float nbSecondsByStep = 0.001f; 

	// Find the corresponding number of steps that must be taken 
	int nbStepsToPerform = static_cast<int>(fTime/nbSecondsByStep); 

	CLogManager::Instance()->LogMessage("stepping ODE");
	
	// Make these steps to advance world time 
	for (int i = 0; i < nbStepsToPerform; i++) 
	{
		ApplyGravity();

		m_iContacts = 0;
		dSpaceCollide( m_oSpace, 0, collideCallback );
		HandleCollisions();

		// Step world
		dWorldQuickStep(m_oWorld, nbSecondsByStep); 
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

void CODEManager::CreatePhysicsData( PhysicsData &d, float fRadius )
{
	if ( d.geom )
		dGeomDestroy(d.geom);
	if ( d.body )
		dBodyDestroy(d.body);

	d.body = CreateBody();
	d.geom = CreateGeom( d.body, fRadius );
	d.m_fGravConst = 0.0f;
	d.m_bAffectedByGravity = true;
	d.m_bHasCollision = true;

	AddData( &d );
}

void CODEManager::CollisionCallback(void *pData, dGeomID o1, dGeomID o2)
{
	PhysicsData *d1 = GetPhysicsDataByGeom(o1);
	PhysicsData *d2 = GetPhysicsDataByGeom(o2);

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

			if ( distance != 0.0f )
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
	for ( int i = 0; i<m_iContacts; i++ )
	{
		dContactGeom c = m_oContacts[i];

		Vector normal = Vector(c.normal);
//		normal *= c.depth * 1000;

		dBodyID b1 = dGeomGetBody(c.g1);
		dBodyID b2 = dGeomGetBody(c.g2);

		Vector v1 = Vector(b1->lvel);
		Vector v2 = Vector(b2->lvel);

		Vector v1New = v1.Mirror( normal );
		Vector v2New = v2.Mirror( normal );

		v1New.CopyInto( b1->lvel );
		v2New.CopyInto( b2->lvel );

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

/*		Vector v1 = Vector(b1->posr.pos);
		Vector v2 = Vector(b2->posr.pos);

		dBodyAddForce( b1, normal[0], normal[1], 0.0f );
		dBodyAddForce( b2, -normal[0], -normal[1], 0.0f );*/
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
