#pragma once
namespace SE
{
	class CCamera
	{
	public:
		CCamera();
		~CCamera();

		bool Init(float aHorizontalFoV, const Vector2f& anAspectRatio);
		bool Init(float2 aResolution);
		void SetTransform(const Vector3f& aPosition, const Vector3f& aRotation);
		void SetTransform(const Matrix4x4f& aTransform);
		void SetRotation(const Vector3f& aRotation);
		void SetPosition(const Vector3f& aPosition);
		void SetCullingRadius(float aRadius);
		const Vector3f GetPosition() const;
		const Vector3f GetRenderOffset() const;
		void SetRenderOffset(const Vector3f& anOffset);
		void Move(const Vector3f& aMovement);
		void Rotate(const Vector3f& aRotation);
		void GetProjectionPlanes(float& aNearPlane, float& aFarPlane);
		void SetOrbit(const float2 aRotation, const float2 anOffset, const float aDistance);
		void SetProjection(const Matrix4x4f aProj) { myProjection = aProj; }
	public:
		const Matrix4x4f& GetTransform() const { return myTransform; }
		const Matrix4x4f& GetProjection() const { return myProjection; }
		__forceinline float& GetCullingRadius()
		{
			return myCullingRadius;
		}
		bool PassesCulling(const Vector3f& aPosition, float aRadius) const;
		void Shake(float anIntensity, float aDuration);
		void StopShaking();
		void Update(float aDeltaTime);

	private:
		Matrix4x4f myTransform;
		Matrix4x4f myProjection;
		float3 myRenderOffset;
		float3 myShake;
		float myShakeDuration;
		float myShakeIntensity;
		float myFarPlane;
		float myNearPlane;
		float myCullingRadius = 3500.0f;
	};
}
