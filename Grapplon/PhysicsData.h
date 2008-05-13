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
	PhysicsData() : body(NULL), geom(NULL), planetData(NULL), m_fAirDragConst(0.0f) { }
	bool CollidesWith( PhysicsData *pData );
	void ToggleIgnore( PhysicsData *pData );

	CBaseObject *m_pOwner;

	dBodyID body;
	dGeomID geom;
	float m_fMass;
	float m_fGravConst;
	float m_fAirDragConst;
	bool m_bAffectedByGravity;
	bool m_bHasCollision;
	PlanetaryData *planetData;

	// Hook specifics
	bool m_bIsHook;

	bool operator ==(PhysicsData &other) { return (this->body == other.body); }
	bool operator !=(PhysicsData &other) { return !(*this == other); }
};
