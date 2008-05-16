#pragma once

#include <string>
#include "Vector.h"
#include <ode/ode.h>

enum PlanetType
{
	SUN,
	NORMAL,
	ICE,
	BROKEN
};

enum OrbitStyle
{
	STATIC,
	CIRCLE,
	ELLIPSE
};

struct PlanetaryData
{
	Vector position;
	dJointID orbitJoint;

	std::string name;
	std::string orbit;
	std::string image;

	PlanetType planetType;
	OrbitStyle orbitStyle;

	float gravconst;
	int start_angle;
	int mass;
	int range;
	float orbitSpeed;
	int asteroidcount;
	int radius;

	float scale;
	int rotation;

	std::string emitter;
	int offsetForward, offsetRight;
	bool bNear;

	bool bIsOrbitting;
};
