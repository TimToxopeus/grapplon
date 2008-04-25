#pragma once

#include <ode/ode.h>
#include <vector>

class PhysicsData
{
private:
	std::vector<PhysicsData *> m_vIgnoreCollisions;

public:
	PhysicsData() { body = NULL; geom = NULL; }
	bool CollidesWith( PhysicsData *pData );
	void ToggleIgnore( PhysicsData *pData );

	dBodyID body;
	dGeomID geom;
	float m_fGravConst;
	bool m_bAffectedByGravity;
	bool m_bHasCollision;
	bool m_bHasPhysics;

	bool operator ==(PhysicsData &other) { if ( this->body == other.body ) return true; return false; }
	bool operator !=(PhysicsData &other) { return !(*this == other); }
};
