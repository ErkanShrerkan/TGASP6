#include "pch.h"
#include "Camera.h"
namespace SE
{
	CCamera::CCamera()
	{

	}

	CCamera::~CCamera()
	{
		
	}

	bool CCamera::Init(float aHorizontalFoV, const Vector2f& anAspectRatio)
	{
		// Convert Degree to Radian
		float horizontalFoVinRadians = Math::DegreeToRadian(aHorizontalFoV);

		// Ideal vertical FoV based on aspect ratio
		float verticalFoVinRadians = 2.f * std::atanf(std::tanf(horizontalFoVinRadians * 0.5f) * (anAspectRatio.height / anAspectRatio.width));

		float myXScale = 1.f / std::tanf(horizontalFoVinRadians * 0.5f);
		float myYScale = 1.f / std::tanf(verticalFoVinRadians * 0.5f);

		myFarPlane =  100000.f; // TODO: Make "global"
		myNearPlane = 100.0f; // TODO: Make "global"

		float planeConstant = myFarPlane / (myFarPlane - myNearPlane);

		myProjection(1, 1) = myXScale;
		myProjection(2, 2) = myYScale;
		myProjection(3, 3) = planeConstant;
		myProjection(3, 4) = 1.f / planeConstant;
		myProjection(4, 3) = -planeConstant * myNearPlane;
		myProjection(4, 4) = 0.0f;

		return true;
	}

	bool CCamera::Init(float2 aResolution)
	{
		myFarPlane = 100000.f;
		myNearPlane = 100.0f;

		myProjection(1, 1) = 2.f / aResolution.x;
		myProjection(2, 2) = 2.f / aResolution.y;
		myProjection(3, 3) = 1.f / (myFarPlane - myNearPlane);
		myProjection(4, 3) = myNearPlane / (myNearPlane - myFarPlane);
		myProjection(4, 4) = 1.0f;

		return true;
	}

	void CCamera::SetTransform(const Vector3f& aPosition, const Vector3f& aRotation)
	{
		SetPosition(aPosition);
		SetRotation(aRotation);
	}

	void CCamera::SetTransform(const Matrix4x4f& aTransform)
	{
		myTransform = aTransform;
	}

	void CCamera::SetRotation(const Vector3f& aRotation)
	{
		Matrix4x4f rotatedMatrix;
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundX(Math::DegreeToRadian(aRotation.x));
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundY(Math::DegreeToRadian(aRotation.y));
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundZ(Math::DegreeToRadian(aRotation.z));
		myTransform.SetRotation(rotatedMatrix);
	}

	void CCamera::SetPosition(const Vector3f& aPosition)
	{
		myTransform.SetTranslation(aPosition);
	}

	void CCamera::SetCullingRadius(float aRadius)
	{
		myCullingRadius = aRadius;
	}

	const Vector3f CCamera::GetPosition() const
	{
		return myTransform.GetTranslation();
	}

	const Vector3f CCamera::GetRenderOffset() const
	{
		return myRenderOffset + myShake;
	}

	void CCamera::SetRenderOffset(const Vector3f& anOffset)
	{
		myRenderOffset = anOffset;
	}

	void CCamera::Move(const Vector3f& aMovement)
	{
		Matrix4x4f translation;
		translation.SetTranslation(aMovement);
		translation *= myTransform;
		myTransform = translation;
	}

	void CCamera::Rotate(const Vector3f& aRotation)
	{
		// This rotation is bound by axis

		Matrix4x4f rotatedMatrix(myTransform);
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundX(Math::DegreeToRadian(aRotation.x));
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundY(Math::DegreeToRadian(aRotation.y));
		rotatedMatrix *= Matrix4x4f::CreateRotationAroundZ(Math::DegreeToRadian(aRotation.z));
		myTransform.SetRotation(rotatedMatrix);
	}

	void CCamera::GetProjectionPlanes(float& aNearPlane, float& aFarPlane)
	{
		aNearPlane = myNearPlane;
		aFarPlane = myFarPlane;
	}

	void CCamera::SetOrbit(const float2 aRotation, const float2 anOffset, const float aDistance)
	{
		Matrix4x4f identity;

		identity *= Matrix4x4f::CreateRotationAroundX(Math::DegreeToRadian(aRotation.x));
		identity *= Matrix4x4f::CreateRotationAroundY(Math::DegreeToRadian(aRotation.y));

		myTransform.SetRotation(identity);

		float3 dir = { myTransform(3, 1), myTransform(3, 2), myTransform(3, 3) };

		dir *= -aDistance;

		dir += myTransform.GetRight() * anOffset.x;
		dir += myTransform.GetUp() * anOffset.y;

		SetPosition(dir);
	}
	bool CCamera::PassesCulling(const Vector3f& aPosition, float aRadius) const
	{
		Vector3f pos = GetPosition() + GetRenderOffset();
		pos += GetTransform().GetForward() * (myCullingRadius * 0.75f);

		Vector3f distance = aPosition - pos;
		float combinedRadius = aRadius + myCullingRadius;
		return (distance.LengthSqr() <= combinedRadius * combinedRadius);
	}
	
	void CCamera::Shake(float anIntensity, float aDuration)
	{
		if (myShakeIntensity < (anIntensity + 0.00001f))
		{
			myShakeDuration = aDuration;
			myShakeIntensity = anIntensity;
		}
	}

	void CCamera::StopShaking()
	{
		myShakeIntensity = 0.0f;
		myShakeDuration = -0.1f;
	}

	void CCamera::Update(float aDeltaTime)
	{
		if (myShakeDuration > 0.0f)
		{
			float x = myShakeDuration * 1000.f;
			
			myShake.x = myShakeIntensity * sin(x * 0.5f) + cos(x * 3.0f) + cos(sin(x * 4.0f) * 3.0f);
			myShake.z = myShakeIntensity * cos(3.0f * (x - 0.25f)) + sin(-5.0f * x) + abs(sin(x * 3.0f)) * 2.0f - 1.0f;
		}
		else
		{
			myShake.SetAll(0.0f);
			myShakeIntensity = 0.0f;
		}

		myShakeDuration -= aDeltaTime;
	}
}
