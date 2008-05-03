#pragma once

#include <ode/ode.h>
#include <vector>

#include "PlanetaryData.h"

class CBaseObject;

class PhysicsData
{
private:
	std::vector<PhysicsData *> m_vIgnoreCollisions;

public:
	PhysicsData() { body = NULL; geom = NULL; planetData = NULL; }
	bool CollidesWith( PhysicsData *pData );
	void ToggleIgnore( PhysicsData *pData );

	CBaseObject *m_pOwner;

	dBodyID body;
	dGeomID geom;
	float m_fMass;
	float m_fGravConst;
	bool m_bAffectedByGravity;
	bool m_bHasCollision;
	bool m_bHasPhysics;

	bool m_bIsPlanet;
	PlanetaryData *planetData;

	// Hook specifics
	bool m_bIsHook;
	dJointID m_oHookGrabJoint;
	PhysicsData *m_pGrabbedObject;

	bool operator ==(PhysicsData &other) { if ( this->body == other.body ) return true; return false; }
	bool operator !=(PhysicsData &other) { return !(*this == other); }
};
