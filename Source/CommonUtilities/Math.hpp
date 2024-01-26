#pragma once
#include <cmath>

namespace Math
{
	constexpr const float pi = 3.14159265358979323851280895940618620443274267017841339111328125f;
	constexpr const float radian = pi / 180.f;
	constexpr const float degree = 180.0f / pi;

	inline constexpr const float DegreeToRadian(const float aDegree)
	{
		return aDegree * radian;
	}

	inline constexpr const float RadianToDegree(const float aRadian)
	{
		return aRadian * degree;
	}

	template <class T>
	inline constexpr const T& Clamp(const T& aValue, const T& aLowerbound, const T& aHigherbound)
	{
		return aValue < aLowerbound ? aLowerbound : aHigherbound < aValue ? aHigherbound : aValue;
	}

	inline constexpr bool IsPowerOfTwo(int& aNumber)
	{
		return aNumber && (!(aNumber & (aNumber - 1)));
	}

	inline constexpr bool IsPowerOfTwo(int aNumber)
	{
		return aNumber && (!(aNumber & (aNumber - 1)));
	}

	template <class T>
	inline const T Lerp(const T& aStartValue, const T& anEndValue, float aPercentage)
	{
		return aStartValue * (1.f - aPercentage) + anEndValue * aPercentage;
	}
}
