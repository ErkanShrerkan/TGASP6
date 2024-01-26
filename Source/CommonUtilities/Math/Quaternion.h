// Copyright (C) 2002-2012 Nikolaus Gebhardt
 // This file is part of the "Irrlicht Engine".
 // For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_QUATERNION_H_INCLUDED__
#define __IRR_QUATERNION_H_INCLUDED__

#include "../pch.h"

// Between Irrlicht 1.7 and Irrlicht 1.8 the quaternion-matrix conversions got fixed.
// This define disables all involved functions completely to allow finding all places 
// where the wrong conversions had been in use.
#define IRR_TEST_BROKEN_QUATERNION_USE 0


class Quaternion
{
public:

	Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}

	Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) { }

	Quaternion(float x, float y, float z);

	Quaternion(const float3& vec);

#if !IRR_TEST_BROKEN_QUATERNION_USE

	Quaternion(const Matrix4x4f& mat);
#endif

	bool operator==(const Quaternion& other) const;

	bool operator!=(const Quaternion& other) const;

	inline Quaternion& operator=(const Quaternion& other);

#if !IRR_TEST_BROKEN_QUATERNION_USE

	inline Quaternion& operator=(const Matrix4x4f& other);
#endif

	Quaternion operator+(const Quaternion& other) const;

	Quaternion operator*(const Quaternion& other) const;

	Quaternion operator*(float s) const;

	Quaternion& operator*=(float s);

	float3 operator*(const float3& v) const;

	Quaternion& operator*=(const Quaternion& other);

	inline float dotProduct(const Quaternion& other) const;

	inline Quaternion& set(float x, float y, float z, float w);

	inline Quaternion& set(float x, float y, float z);

	inline Quaternion& set(const float3& vec);

	inline Quaternion& set(const Quaternion& quat);

	inline Quaternion& normalize();

#if !IRR_TEST_BROKEN_QUATERNION_USE

	Matrix4x4f getMatrix() const;
	void getRotationMatrix(Matrix4x4f& dest) const;
#endif

	void getMatrix(Matrix4x4f& dest, const float3& translation = float3()) const;

	void getMatrixCenter(Matrix4x4f& dest, const float3& center, const float3& translation) const;

	inline void getMatrix_transposed(Matrix4x4f& dest) const;

	Quaternion& makeInverse();


	Quaternion& lerp(Quaternion q1, Quaternion q2, float time);


	Quaternion& slerp(Quaternion q1, Quaternion q2,
		float time, float threshold = .05f);


	Quaternion& fromAngleAxis(float angle, const float3& axis);

	void toAngleAxis(float& angle, float3& axis) const;

	Quaternion& makeIdentity();

	Quaternion& rotationFromTo(const float3& from, const float3& to);

	float x; // vectorial (imaginary) part
	float y;
	float z;
	float w; // real part
};


// Constructor which converts euler angles to a quaternion
inline Quaternion::Quaternion(float x, float y, float z)
{
	set(x, y, z);
}


// Constructor which converts euler angles to a quaternion
inline Quaternion::Quaternion(const float3& vec)
{
	set(vec.x, vec.y, vec.z);
}

#if !IRR_TEST_BROKEN_QUATERNION_USE
// Constructor which converts a matrix to a quaternion
inline Quaternion::Quaternion(const Matrix4x4f& mat)
{
	(*this) = mat;
}
#endif

// equal operator
inline bool Quaternion::operator==(const Quaternion& other) const
{
	return ((x == other.x) &&
		(y == other.y) &&
		(z == other.z) &&
		(w == other.w));
}

// inequality operator
inline bool Quaternion::operator!=(const Quaternion& other) const
{
	return !(*this == other);
}

// assignment operator
inline Quaternion& Quaternion::operator=(const Quaternion& other)
{

	x = other.x;

	y = other.y;

	z = other.z;

	w = other.w;

	return *this;

}

#if !IRR_TEST_BROKEN_QUATERNION_USE
// matrix assignment operator
inline Quaternion& Quaternion::operator=(const Matrix4x4f& m)
{
	const float diag = m[0] + m[5] + m[10] + 1;

	if (diag > 0.0f)
	{
		const float scale = sqrtf(diag) * 2.0f; // get scale from diagonal

		// TODO: speed this up
		x = (m[6] - m[9]) / scale;
		y = (m[8] - m[2]) / scale;
		z = (m[1] - m[4]) / scale;
		w = 0.25f * scale;
	}
	else
	{
		if (m[0] > m[5] && m[0] > m[10])
		{
			// 1st element of diag is greatest value
			// find scale according to 1st element, and double it
			const float scale = sqrtf(1.0f + m[0] - m[5] - m[10]) * 2.0f;

			// TODO: speed this up
			x = 0.25f * scale;
			y = (m[4] + m[1]) / scale;
			z = (m[2] + m[8]) / scale;
			w = (m[6] - m[9]) / scale;
		}
		else if (m[5] > m[10])
		{
			// 2nd element of diag is greatest value
			// find scale according to 2nd element, and double it
			const float scale = sqrtf(1.0f + m[5] - m[0] - m[10]) * 2.0f;

			// TODO: speed this up
			x = (m[4] + m[1]) / scale;
			y = 0.25f * scale;
			z = (m[9] + m[6]) / scale;
			w = (m[8] - m[2]) / scale;
		}
		else
		{
			// 3rd element of diag is greatest value
			// find scale according to 3rd element, and double it
			const float scale = sqrtf(1.0f + m[10] - m[0] - m[5]) * 2.0f;

			// TODO: speed this up
			x = (m[8] + m[2]) / scale;
			y = (m[9] + m[6]) / scale;
			z = 0.25f * scale;
			w = (m[1] - m[4]) / scale;
		}
	}

	return normalize();
}
#endif

inline void Quaternion::getRotationMatrix(Matrix4x4f& dest) const
{
	dest[0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
	dest[1] = 2.0f * x * y + 2.0f * z * w;
	dest[2] = 2.0f * x * z - 2.0f * y * w;
	dest[3] = 0.0f;

	dest[4] = 2.0f * x * y - 2.0f * z * w;
	dest[5] = 1.0f - 2.0f * x * x - 2.0f * z * z;
	dest[6] = 2.0f * z * y + 2.0f * x * w;
	dest[7] = 0.0f;

	dest[8] = 2.0f * x * z + 2.0f * y * w;
	dest[9] = 2.0f * z * y - 2.0f * x * w;
	dest[10] = 1.0f - 2.0f * x * x - 2.0f * y * y;
	dest[11] = 0.0f;

	dest[12] = 0;
	dest[13] = 0;
	dest[14] = 0;
	dest[15] = 1.f;
}

// multiplication operator
inline Quaternion Quaternion::operator*(const Quaternion& other) const
{
	Quaternion tmp;

	tmp.w = (other.w * w) - (other.x * x) - (other.y * y) - (other.z * z);
	tmp.x = (other.w * x) + (other.x * w) + (other.y * z) - (other.z * y);
	tmp.y = (other.w * y) + (other.y * w) + (other.z * x) - (other.x * z);
	tmp.z = (other.w * z) + (other.z * w) + (other.x * y) - (other.y * x);

	return tmp;
}

// multiplication operator
inline Quaternion Quaternion::operator*(float s) const
{
	return Quaternion(s * x, s * y, s * z, s * w);
}

// multiplication operator
inline Quaternion& Quaternion::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	w *= s;
	return *this;
}

// multiplication operator
inline Quaternion& Quaternion::operator*=(const Quaternion& other)
{
	return (*this = other * (*this));
}

// add operator
inline Quaternion Quaternion::operator+(const Quaternion& b) const
{
	return Quaternion(x + b.x, y + b.y, z + b.z, w + b.w);
}

#if !IRR_TEST_BROKEN_QUATERNION_USE
// Creates a matrix from this quaternion
inline Matrix4x4f Quaternion::getMatrix() const
{
	Matrix4x4f m;
	getMatrix(m);
	return m;
}
#endif

inline void Quaternion::getMatrix(Matrix4x4f& dest,
	const float3& center) const
{
	dest[0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
	dest[1] = 2.0f * x * y + 2.0f * z * w;
	dest[2] = 2.0f * x * z - 2.0f * y * w;
	dest[3] = 0.0f;

	dest[4] = 2.0f * x * y - 2.0f * z * w;
	dest[5] = 1.0f - 2.0f * x * x - 2.0f * z * z;
	dest[6] = 2.0f * z * y + 2.0f * x * w;
	dest[7] = 0.0f;

	dest[8] = 2.0f * x * z + 2.0f * y * w;
	dest[9] = 2.0f * z * y - 2.0f * x * w;
	dest[10] = 1.0f - 2.0f * x * x - 2.0f * y * y;
	dest[11] = 0.0f;

	dest[12] = center.x;
	dest[13] = center.y;
	dest[14] = center.z;
	dest[15] = 1.f;
}

inline void Quaternion::getMatrixCenter(Matrix4x4f& dest,
	const float3& center,
	const float3& translation) const
{
	center;
	translation;
	dest[0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
	dest[1] = 2.0f * x * y + 2.0f * z * w;
	dest[2] = 2.0f * x * z - 2.0f * y * w;
	dest[3] = 0.0f;

	dest[4] = 2.0f * x * y - 2.0f * z * w;
	dest[5] = 1.0f - 2.0f * x * x - 2.0f * z * z;
	dest[6] = 2.0f * z * y + 2.0f * x * w;
	dest[7] = 0.0f;

	dest[8] = 2.0f * x * z + 2.0f * y * w;
	dest[9] = 2.0f * z * y - 2.0f * x * w;
	dest[10] = 1.0f - 2.0f * x * x - 2.0f * y * y;
	dest[11] = 0.0f;
}

// Creates a matrix from this quaternion
inline void Quaternion::getMatrix_transposed(Matrix4x4f& dest) const
{
	dest[0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
	dest[4] = 2.0f * x * y + 2.0f * z * w;
	dest[8] = 2.0f * x * z - 2.0f * y * w;
	dest[12] = 0.0f;

	dest[1] = 2.0f * x * y - 2.0f * z * w;
	dest[5] = 1.0f - 2.0f * x * x - 2.0f * z * z;
	dest[9] = 2.0f * z * y + 2.0f * x * w;
	dest[13] = 0.0f;

	dest[2] = 2.0f * x * z + 2.0f * y * w;
	dest[6] = 2.0f * z * y - 2.0f * x * w;
	dest[10] = 1.0f - 2.0f * x * x - 2.0f * y * y;
	dest[14] = 0.0f;

	dest[3] = 0.f;
	dest[7] = 0.f;
	dest[11] = 0.f;
	dest[15] = 1.f;
}


// Inverts this quaternion
inline Quaternion& Quaternion::makeInverse()
{
	x = -x; y = -y; z = -z;
	return *this;
}


// sets new quaternion
inline Quaternion& Quaternion::set(float X, float Y, float Z, float W)
{
	x = X;
	y = Y;
	z = Z;
	w = W;
	return *this;
}


// sets new quaternion based on euler angles
inline Quaternion& Quaternion::set(float X, float Y, float Z)
{
	double angle;

	angle = X * 0.5;
	const double sr = sin(angle);
	const double cr = cos(angle);

	angle = Y * 0.5;
	const double sp = sin(angle);
	const double cp = cos(angle);

	angle = Z * 0.5;
	const double sy = sin(angle);
	const double cy = cos(angle);

	const double cpcy = cp * cy;
	const double spcy = sp * cy;
	const double cpsy = cp * sy;
	const double spsy = sp * sy;

	x = (float)(sr * cpcy - cr * spsy);
	y = (float)(cr * spcy + sr * cpsy);
	z = (float)(cr * cpsy - sr * spcy);
	w = (float)(cr * cpcy + sr * spsy);

	return normalize();
}

// sets new quaternion based on euler angles
inline Quaternion& Quaternion::set(const float3& vec)
{
	return set(vec.x, vec.y, vec.z);
}

// sets new quaternion based on other quaternion
inline Quaternion& Quaternion::set(const Quaternion& quat)
{
	return (*this = quat);
}

// normalizes the quaternion
inline Quaternion& Quaternion::normalize()
{
	const float n = x * x + y * y + z * z + w * w;

	if (n == 1)
		return *this;

	//n = 1.0f / sqrtf(n);
	return (*this *= (1.f / sqrt(n)));
}

// set this quaternion to the result of the linear interpolation between two quaternions
inline Quaternion& Quaternion::lerp(Quaternion q1, Quaternion q2, float time)
{
	const float scale = 1.0f - time;
	return (*this = (q1 * scale) + (q2 * time));
}


// set this quaternion to the result of the interpolation between two quaternions
inline Quaternion& Quaternion::slerp(Quaternion q1, Quaternion q2, float time, float threshold)
{
	float angle = q1.dotProduct(q2);

	// make sure we use the short rotation
	if (angle < 0.0f)
	{
		q1 *= -1.0f;
		angle *= -1.0f;
	}

	if (angle <= (1 - threshold)) // spherical interpolation
	{
		const float theta = acosf(angle);
		const float invsintheta = 1.f / (sinf(theta));
		const float scale = sinf(theta * (1.0f - time)) * invsintheta;
		const float invscale = sinf(theta * time) * invsintheta;
		return (*this = (q1 * scale) + (q2 * invscale));
	}
	else // linear interploation
		return lerp(q1, q2, time);
}


// calculates the dot product
inline float Quaternion::dotProduct(const Quaternion& q2) const
{
	return (x * q2.x) + (y * q2.y) + (z * q2.z) + (w * q2.w);
}


inline Quaternion& Quaternion::fromAngleAxis(float angle, const float3& axis)
{
	const float fHalfAngle = 0.5f * angle;
	const float fSin = sinf(fHalfAngle);
	w = cosf(fHalfAngle);
	x = fSin * axis.x;
	y = fSin * axis.y;
	z = fSin * axis.z;
	return *this;
}


inline void Quaternion::toAngleAxis(float& angle, float3& axis) const
{
	const float scale = sqrtf(x * x + y * y + z * z);

	if (abs(scale) < 0.0001 || w > 1.0f || w < -1.0f)
	{
		angle = 0.0f;
		axis.x = 0.0f;
		axis.y = 1.0f;
		axis.z = 0.0f;
	}
	else
	{
		const float invscale = 1.f / (scale);
		angle = 2.0f * acosf(w);
		axis.x = x * invscale;
		axis.y = y * invscale;
		axis.z = z * invscale;
	}
}


inline float3 Quaternion::operator* (const float3& v) const
{
	// nVidia SDK implementation

	float3 uv, uuv;
	float3 qvec(x, y, z);
	uv = qvec.Cross(v);
	uuv = qvec.Cross(uv);
	uv *= (2.0f * w);
	uuv *= 2.0f;

	return v + uv + uuv;
}

// set quaternion to identity
inline Quaternion& Quaternion::makeIdentity()
{
	w = 1.f;
	x = 0.f;
	y = 0.f;
	z = 0.f;
	return *this;
}

inline Quaternion& Quaternion::rotationFromTo(const float3& from, const float3& to)
{
	// Based on Stan Melax's article in Game Programming Gems
	// Copy, since cannot modify local
	float3 v0 = from;
	float3 v1 = to;
	v0.Normalize();
	v1.Normalize();

	const float d = v0.Dot(v1);
	if (d >= 1.0f) // If dot == 1, vectors are the same
	{
		return makeIdentity();
	}
	else if (d <= -1.0f) // exactly opposite
	{
		float3 axis(1.0f, 0.f, 0.f);
		axis = axis.Cross(v0);
		if (axis.Length() == 0)
		{
			axis = { 0.f, 1.f, 0.f };
			axis = axis.Cross(v0);
		}
		// same as fromAngleAxis(PI, axis).normalize();
		return set(axis.x, axis.y, axis.z, 0).normalize();
	}

	const float s = sqrtf((1 + d) * 2); // optimize inv_sqrt
	const float invs = 1.f / s;
	const float3 c = v0.Cross(v1) * invs;
	return set(c.x, c.y, c.z, s * 0.5f).normalize();
}
#endif
