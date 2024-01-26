#pragma once

namespace SE
{
    class CTransform
    {
	public:
		void SetTransform(const Vector3f& aPosition, const Vector3f& aRotation);
		void SetTransform(const Matrix4x4f& aTransform);
		Matrix4x4f& GetTransform();
		void SetRotation(const Vector3f& aRotation);
		void SetPosition(const Vector3f& aPosition);
		const Vector3f GetPosition() const;
		const Vector3f GetRotation() const;
		void Move(const Vector3f& aMovement);
		void Rotate(const Vector3f& aRotation);

		inline void SetScale(const Vector3f& aScale)
		{
			myScale = aScale;
		}
		inline Vector3f& GetScale()
		{
			return myScale;
		}

		__forceinline const Matrix4x4f& GetMatrix() const
		{
			return myTransform;
		}
		__forceinline Matrix4x4f& GetMatrix()
		{
			return myTransform;
		}

	private:
		Matrix4x4f myTransform;
		Vector3f myScale = Vector3f::One();
    };
}
