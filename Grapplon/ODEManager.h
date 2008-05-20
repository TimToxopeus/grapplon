#pragma once

#include <ode/ode.h>
#include <vector>
//#include "ode/objects.h"
#include "PhysicsData.h"

//Forward declarations
class CBaseObject;

#define ODE_DEPTH 10
#define MAX_CONTACTS 128
#define MAX_HINGES 64

class CODEManager
{
private:
	static CODEManager *m_pInstance;
	CODEManager();
	virtual ~CODEManager();

	dWorldID m_oWorld;
	dSpaceID m_oSpace;
	dJointGroupID m_oContactgroup;
	dJointGroupID m_oJointgroup;

	std::vector<dJointID> m_vJoints;

	dContactGeom m_oContacts[MAX_CONTACTS];
	int m_iContacts;

	dBodyID CreateBody();
	dGeomID CreateGeom( dBodyID body, float fRadius );
	void AddData( PhysicsData *pData );

	void ApplyGravity();
	void ApplyMotorForceAndDrag();

	void HandleCollisions();

public:

	std::vector<PhysicsData *> m_vAsteroids;
	std::vector<PhysicsData *> m_vPlanets;	
	std::vector<PhysicsData *> m_vPlayers;	
	std::vector<PhysicsData *> m_vOthers;

	static CODEManager *Instance() { if ( !m_pInstance ) m_pInstance = new CODEManager(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	void CreatePhysicsData( CBaseObject *pOwner, PhysicsData* d, float fRadius = 70.0f);

	void Update( float fTime );
	dJointID createHingeJoint(char* name);
	dJointID CreateJoint( dBodyID b1, dBodyID b2, float x = 0, float y = 0 );
	void DestroyJoint( dJointID joint );

	int m_iWidth;
	int m_iHeight;
	int m_iBoundryForce;

	const dWorldID& getWorld() { return m_oWorld; };
	const dSpaceID& getSpace() { return m_oSpace; };

	void CollisionCallback( void *pData, dGeomID o1, dGeomID o2 );
};
