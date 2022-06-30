#include "unity_structs.h"


// vector3f
// casting
vector3f::operator float* ()
{
	return (float*)&x;
}

vector3f::operator const float* () const
{
	return (const float*)&x;
}


// assignment operators
vector3f& vector3f::operator += (const vector3f& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

vector3f& vector3f::operator -= (const vector3f& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

vector3f& vector3f::operator *= (float f)
{
	x *= f;
	y *= f;
	z *= f;
	return *this;
}

vector3f& vector3f::operator /= (float f)
{
	float fInv = 1.0f / f;
	x *= fInv;
	y *= fInv;
	z *= fInv;
	return *this;
}


// binary operators
bool vector3f::operator == (const vector3f& v) const
{
	return x == v.x && y == v.y && z == v.z;
}

bool vector3f::operator != (const vector3f& v) const
{
	return x != v.x || y != v.y || z != v.z;
}



// functions
float vector3f::distance(const vector3f& vec)
{
	// distance formula (algebra):
	//		*** d = sqrt((x1 - x2)^2 + (y1 - y2)^2) ***
	// reason for sqrt(dist^2) is because that is same
	// for absolute value. you cannot have "negative distance"

	//float fx = x - vec.x, fy = y - vec.y, fz = z - vec.z;
	//return sqrtf((fx * fx) + (fy * fy) + (fz * fz));
	return sqrtf((*this - vec).magnitude());
}


float vector3f::magnitude()
{
	return sqrtf((x * x) + (y * y) + (z * z));
}

vector3f vector3f::normalize()
{
	// ngl not sure how this works
	float mgn = magnitude();
	if (mgn > 1E-05f)
		return *this / mgn;
	return vector3f();
}