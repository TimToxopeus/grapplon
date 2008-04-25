#pragma once

#include <ode/ode.h>
#include <vector>
#include "ode/objects.h"
#include "PhysicsData.h"

//Forward declarations
class CBaseObject;

#define ODE_DEPTH 10
#define MAX_CONTACTS 64

class CODEManager
{
private:
	static CODEManager *m_pInstance;
	CODEManager();
	virtual ~CODEManager();

	dWorldID m_oWorld;
	dSpaceID m_oSpace; 

	std::vector<PhysicsData *> m_vBodies;

	dContactGeom m_oContacts[MAX_CONTACTS];
	int m_iContacts;

	dBodyID CreateBody();
	dGeomID CreateGeom( dBodyID body, float fRadius );
	void AddData( PhysicsData *pData );

	void ApplyGravity();
	void HandleCollisions();

	PhysicsData *GetPhysicsDataByGeom( dGeomID o );

public:
	static CODEManager *Instance() { if ( !m_pInstance ) m_pInstance = new CODEManager(); return m_pInstance; }
	static void Destroy() { if ( m_pInstance ) { delete m_pInstance; m_pInstance = 0; } }

	void CreatePhysicsData( PhysicsData &d, float fRadius = 70.0f );
	void Update( float fTime );

	dJointGroupID m_oContactgroup;

	const dWorldID& getWorld() { return m_oWorld; };
	const dSpaceID& getSpace() { return m_oSpace; };

	void CollisionCallback( void *pData, dGeomID o1, dGeomID o2 );
};
