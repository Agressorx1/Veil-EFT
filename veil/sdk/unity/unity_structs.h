#pragma once
#include <math.h>


// unity structures
typedef struct vector3f
{
public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

public:
	vector3f() {}
	vector3f(const float* pf)
	{
		if (!pf)
			return;

		x = pf[0];
		y = pf[1];
		z = pf[2];
	}
	vector3f(const struct vector3f& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
	}
	vector3f(float fx, float fy, float fz)
	{
		x = fx;
		y = fy;
		z = fz;
	}

public:
	// casting
	operator float* ();
	operator const float* () const;

public:
	// assignment operators
	inline vector3f& operator += (const vector3f& v);
	inline vector3f& operator -= (const vector3f& v);
	inline vector3f& operator *= (float f);
	inline vector3f& operator /= (float f);

	// unary operators
	inline vector3f operator + () const
	{ return *this; }
	inline vector3f operator - () const
	{ return vector3f(-x, -y, -z); }

	// binary operators
	inline vector3f operator + (const vector3f& v) const
	{ return vector3f(x + v.x, y + v.y, z + v.z); }
	inline vector3f operator - (const vector3f& v) const
	{ return vector3f(x - v.x, y - v.y, z - v.z); }
	inline vector3f operator * (float f) const
	{ return vector3f(x * f, y * f, z * f); }
	inline vector3f operator / (float f) const
	{ float fInv = 1.0f / f; return vector3f(x * fInv, y * fInv, z * fInv); }

	friend vector3f operator*(float f, const vector3f& v)
	{ return vector3f(f * v.x, f * v.y, f * v.z); }

	bool operator == (const vector3f&) const;
	bool operator != (const vector3f&) const;

public:
	float distance(const vector3f& vec);
	float magnitude();

	vector3f normalize();
};