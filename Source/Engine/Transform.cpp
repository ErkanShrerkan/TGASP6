#include "pch.h"
#include "Transform.h"
#include <CommonUtilities\Math.hpp>

namespace SE
{
	void CTransform::SetTransform(const Vector3f& aPosition, const Vector3f& aRotation)
	{
		SetRotation(aRotation);
		SetPosition(aPosition);
	}

	void CTransform::SetTransform(const Matrix4x4f& aTransform)
	{
		myTransform = aTransform;
	}

	Matrix4x4f& CTransform::GetTransform()
	{
		return myTransform;
	}

	void CTransform::SetRotation(const Vector3f& aRotation)
	{
		Matrix4x4f rotatedMatrix;
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundZ(Math::DegreeToRadian(aRotation.z));
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundX(Math::DegreeToRadian(aRotation.x));
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundY(Math::DegreeToRadian(aRotation.y));
		myTransform.SetRotation(rotatedMatrix);
	}

	void CTransform::SetPosition(const Vector3f& aPosition)
	{
		myTransform.SetTranslation(aPosition);
	}

	const Vector3f CTransform::GetPosition() const
	{
		return myTransform.GetTranslation();
	}

	const Vector3f CTransform::GetRotation() const
	{
		Vector3f radianRotation = myTransform.GetRotation();
		return Vector3f(
			Math::RadianToDegree(radianRotation.x),
			Math::RadianToDegree(radianRotation.y),
			Math::RadianToDegree(radianRotation.z)
		);
	}

	void CTransform::Move(const Vector3f& aMovement)
	{
		Matrix4x4f translation;
		translation.SetTranslation(aMovement);
		translation *= myTransform;
		myTransform = translation;
	}

	void CTransform::Rotate(const Vector3f& aRotation)
	{
		// Relation based rotation, not bound to axis

		Matrix4x4f rotatedMatrix(myTransform);
		rotatedMatrix = Matrix4x4f::CreateRotationAroundX(Math::DegreeToRadian(aRotation.x)) * rotatedMatrix;
		rotatedMatrix = Matrix4x4f::CreateRotationAroundY(Math::DegreeToRadian(aRotation.y)) * rotatedMatrix;
		rotatedMatrix = Matrix4x4f::CreateRotationAroundZ(Math::DegreeToRadian(aRotation.z)) * rotatedMatrix;
		myTransform.SetRotation(rotatedMatrix);
	}
}
