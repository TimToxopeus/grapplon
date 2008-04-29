#pragma once

#include <string>
#include "Vector.h"
#include <ode/ode.h>

struct PlanetaryData
{
	Vector position;
	dJointID orbitJoint;

	std::string name;
	std::string orbit;
	std::string image;

	float gravconst;
	int start_angle;
	int mass;
	int range;
	int revolutiontime;
	int asteroidcount;
	int radius;
};
