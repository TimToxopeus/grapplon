#include "ODEManager.h"
#include "LogManager.h"
#include "BaseObject.h"
#include "Planet.h"
#include <iostream>
#include <string>
#include <sstream>
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

dGeomID CODEManager::CreateGeom( dBodyID body )
{
	dGeomID geom = dCreateSphere(m_oSpace, 70.0f); 
	dGeomSetBody(geom, body);
	return geom;
}

void CODEManager::CollisionCallback(void *pData, dGeomID o1, dGeomID o2)
{
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
		m_iContacts += dCollide( o1, o2, MAX_CONTACTS - m_iContacts, m_oContacts + m_iContacts, sizeof(dContactGeom) );
		// add these contact points to the simulation ...
	}
}

void CODEManager::CreatePhysicsData( PhysicsData &d )
{
	d.body = CreateBody();
	d.geom = CreateGeom( d.body );
	d.m_fGravConst = 0.0f;
	m_vBodies.push_back(&d);
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

			if(object == planet) continue;
			if (object.m_fGravConst > 0.0f ) continue;

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

/*		Vector v1 = Vector(b1->posr.pos);
		Vector v2 = Vector(b2->posr.pos);

		dBodyAddForce( b1, normal[0], normal[1], 0.0f );
		dBodyAddForce( b2, -normal[0], -normal[1], 0.0f );*/
	}
}
