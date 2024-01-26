#pragma once
#include <cmath>
namespace CommonUtilities
{
	template <class T>
	class Vector2
	{
	public:
		union
		{
			T x;
			// Alias for x
			T u, width, _1;
		};
		union
		{
			T y;
			// Alias for y
			T v, height, _2;
		};

		//Creates a null-vector
		Vector2<T>()
		{
			x = T();
			y = T();
		}

		//Creates a vector (aX, aY)
		Vector2<T>(const T& aX, const T& aY)
		{
			x = aX;
			y = aY;
		}

		//Copy constructor (compiler generated)
		Vector2<T>(const Vector2<T>& aVector) = default;

		//Assignment operator (compiler generated)
		Vector2<T>& operator=(const Vector2<T>& aVector2) = default;

		//Destructor (compiler generated)
		~Vector2<T>() = default;

		//Returns the squared length of the vector
		T LengthSqr() const
		{
			return x * x + y * y;
		}

		//Returns the length of the vector
		T Length() const
		{
			return sqrt(x * x + y * y);
		}

		//Returns a normalized copy of this
		Vector2<T> GetNormalized() const
		{
			if (x == T() && y == T())
			{
				return Vector2();
			}

			Vector2<T> temp(x, y);
			T length = Length();
			temp.x /= length;
			temp.y /= length;
			return temp;
		}

		//Normalizes the vector
		void Normalize()
		{
			*this = (*this).GetNormalized();
		}

		//Returns the dot product of this and aVector
		T Dot(const Vector2<T>& aVector) const
		{
			T temp = x * aVector.x;
			temp += y * aVector.y;
			return temp;
		}

		//A Vector with value of 1 in each axis
		static constexpr Vector2<T> One()
		{
			return Vector2<T>(static_cast<T>(1), static_cast<T>(1));
		}

		template <class U>
		Vector2<U> AsVector()
		{
			return Vector2<U>(
				static_cast<U>(x),
				static_cast<U>(y)
			);
		}

		// Sets all components (x, y) to aValue
		void SetAll(const T& aValue)
		{
			x = aValue;
			y = aValue;
		}
	};

	//Returns the vector sum of aVector0 and aVector1
	template<class T> Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		Vector2<T> temp = aVector0;
		temp.x += aVector1.x;
		temp.y += aVector1.y;
		return temp;
	}

	//Returns the vector difference of aVector0 and aVector1
	template<class T> Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		Vector2<T> temp = aVector0;
		temp.x -= aVector1.x;
		temp.y -= aVector1.y;
		return temp;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template<class T> Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
	{
		Vector2<T> temp = aVector;
		temp.x *= aScalar;
		temp.y *= aScalar;
		return temp;
	}

	//Returns the vector aVector multiplied by the scalar aScalar
	template<class T> Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
	{
		Vector2<T> temp = aVector;
		temp.x *= aScalar;
		temp.y *= aScalar;
		return temp;
	}

	//Returns the vector aVector divided by the scalar aScalar (equivalent to aVector multiplied by 1/aScalar)
	template<class T> Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
	{
		Vector2<T> temp = aVector;
		temp.x /= aScalar;
		temp.y /= aScalar;
		return temp;
	}

	//Equivalent to setting aVector0 to (aVector0 + aVector1)
	template<class T> void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x += aVector1.x;
		aVector0.y += aVector1.y;
	}

	//Equivalent to setting aVector0 to (aVector0 - aVector1)
	template<class T> void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
	{
		aVector0.x -= aVector1.x;
		aVector0.y -= aVector1.y;
	}

	//Equivalentto setting aVector to (aVector * aScalar)
	template<class T> void operator*=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector.x *= aScalar;
		aVector.y *= aScalar;
	}

	//Equivalent to setting aVector to (aVector / aScalar)
	template<class T> void operator/=(Vector2<T>& aVector, const T& aScalar)
	{
		aVector.x /= aScalar;
		aVector.y /= aScalar;
	}
}