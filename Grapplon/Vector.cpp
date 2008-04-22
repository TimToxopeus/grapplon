#include "Vector.h"
#include <Math.h>

Vector::Vector()
{
	vector[0] = vector[1] = vector[2] = 0.0f;
}

Vector::Vector( float *vector )
{
	this->vector[0] = vector[0];
	this->vector[1] = vector[1];
	this->vector[2] = vector[2];
}

void Vector::Normalize()
{
	float l = Length();
	if ( l != 0.0f )
	{
		vector[0] /= l;
		vector[1] /= l;
		vector[2] /= l;
	}
}

float Vector::Length()
{
	return (float)sqrt(LengthSquared());
}

float Vector::LengthSquared()
{
	return vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2];
}
