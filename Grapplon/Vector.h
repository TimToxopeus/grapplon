#pragma once

class Vector
{
public:
	Vector();
	Vector( float *vector );
	Vector( float x, float y, float z );

	void Normalize();
	Vector GetNormalized();
	float Length();
	float LengthSquared();
	float DotProduct( Vector &other );
	float Angle( Vector &other );
	Vector CrossProduct( Vector &other );
	Vector Mirror( Vector &normal );

	void CopyInto( float *v ) { v[0] = vector[0]; v[1] = vector[1]; v[2] = vector[2]; }

	float vector[3];

	// Operators
	Vector operator+( Vector &other );
	Vector operator-( Vector &other );
	Vector operator*( float &other );
	Vector operator/( float &other );
	Vector operator*( const float &other );
	Vector operator/( const float &other );

	Vector &operator+=( const Vector &other );
	Vector &operator-=( const Vector &other );
	Vector &operator*=( const float &other );
	Vector &operator/=( const float &other );

	Vector &operator=( const Vector &other );
	Vector &operator=( const float *other );

	float &operator[] (unsigned i);
};
