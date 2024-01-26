#pragma once
#include <cmath>
#include "Vector2.hpp"
namespace CommonUtilities
{
	template <class T>
	class Vector3
	{
	public:

		#pragma warning(disable:4201)
		union
		{
			struct
			{
				union
				{
					T x;
					// Alias for x
					T r, red, _1;
				};
				union
				{
					T y;
					// Alias for y
					T g, green, _2;
				};
			};
			Vector2<T> xy;
		};
		#pragma warning(default:4201)
		union
		{
			T z;
			// Alias for z
			T b, blue, _3;
		};

		//Creates a null-vector
		Vector3<T>()
		{
			x = T();
			y = T();
			z = T();
		}

		//Creates a vector (aX, aY, aZ)
		Vector3<T>(const T& aX, const T& aY, const T& aZ)
		{
			x = aX;
			y = aY;
			z = aZ;
		}

		//Copy constructor (compiler generated)
		Vector3<T>(const Vector3<T>& aVector) = default;
		
		//Assignment operator (compiler generated)
		Vector3<T>& operator=(const Vector3<T>& aVector3) = default;

		//Destructor (compiler generated)
		~Vector3<T>() = default;

		//Returns the squared length of the vector
		T LengthSqr() const
		{
			return x * x + y * y + z * z;
		}

		//Returns the length of the vector
		T Length() const
		{
			return sqrt(x * x + y * y + z * z);
		}

		//Returns a normalized copy of this
		Vector3<T> GetNormalized() const
		{
			if (x == T() && y == T() && z == T())
			{
				return Vector3();
			}

			Vector3<T> temp(x, y, z);
			T length = Length();
			temp.x /= length;
			temp.y /= length;
			temp.z /= length;
			return temp;
		}

		//Normalizes the vector
		void Normalize()
		{
			*this = (*this).GetNormalized();
		}

		//Returns the dot product of this and aVector
		T Dot(const Vector3<T>& aVector) const
		{
			T temp = x * aVector.x;
			temp += y * aVector.y;
			temp += z * aVector.z;
			return temp;
		}

		//Returns the cross product of this and aVector
		Vector3<T> Cross(const Vector3<T>& aVector) const
		{
			Vector3<T> temp;
			temp.x = y * aVector.z - aVector.y * z;
			temp.y = z * aVector.x - aVector.z * x;
			temp.z = x * aVector.y - aVector.x * y;
			return temp;
		}

		//A Vector with value of 1 in each axis
		static constexpr Vector3<T> One()
		{
			return Vector3<T>(static_cast<T>(1), static_cast<T>(1), static_cast<T>(1));
		}

		template <class U>
		Vector3<U> AsVector()
		{
			return Vector3<U>(
				static_cast<U>(x),
				static_cast<U>(y),
				static_cast<U>(z)
			);
		}

		// Sets all components (x, y, z) to aValue
		void SetAll(const T& aValue)
		{
			x = aValue;
			y = aValue;
			z = aValue;
		}
	};

	//Returns the vector sum of aVector0 and aVector1
	template<class T> Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		Vector3<T> temp = aVector0;
		temp.x += aVector1.x;
		temp.y += aVector1.y;
		temp.z += aVector1.z;
		return temp;
	}
	
	//Returns the vector difference of aVector0 and aVector1
	template<class T> Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		Vector3<T> temp = aVector0;
		temp.x -= aVector1.x;
		temp.y -= aVector1.y;
		temp.z -= aVector1.z;
		return temp;
	}
	
	//Returns the vector aVector multiplied by the scalar aScalar
	template<class T> Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
	{
		Vector3<T> temp = aVector;
		temp.x *= aScalar;
		temp.y *= aScalar;
		temp.z *= aScalar;
		return temp;
	}
	
	//Returns the vector aVector multiplied by the scalar aScalar
	template<class T> Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
	{
		Vector3<T> temp = aVector;
		temp.x *= aScalar;
		temp.y *= aScalar;
		temp.z *= aScalar;
		return temp;
	}
	
	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template<class T> Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
	{
		Vector3<T> temp = aVector;
		temp.x /= aScalar;
		temp.y /= aScalar;
		temp.z /= aScalar;
		return temp;
	}
	
	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template<class T> void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
		aVector0.z += aVector1.z;
	}
	
	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template<class T> void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
		aVector0.z -= aVector1.z;
	}
	
	//Equivalentto setting aVector to (aVector * aScalar)
	template<class T> void operator*=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
		aVector.z *= aScalar;
	}
	
	//Equivalent to setting aVector to (aVector / aScalar)
	template<class T> void operator/=(Vector3<T>& aVector, const T& aScalar)
	{
		aVector.x /= aScalar;
		aVector.y /= aScalar;
		aVector.z /= aScalar;
	}
}