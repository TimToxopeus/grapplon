#pragma once

#include <ode/ode.h>

struct PhysicsData
{
	dBodyID body;
	dGeomID geom;
	float m_fGravConst;

	bool operator ==(PhysicsData &other) { if ( this->body == other.body ) return true; return false; }
	bool operator !=(PhysicsData &other) { return !(*this == other); }
};
