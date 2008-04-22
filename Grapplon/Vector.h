#pragma once

class Vector
{
public:
	Vector();
	Vector( float *vector );

	void Normalize();
	float Length();
	float LengthSquared();

	float vector[3];
};
