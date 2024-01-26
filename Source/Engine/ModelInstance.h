#pragma once
#include "Transform.h"

namespace SE
{
	class CAnimator;
	class CModel;
	class CModelInstance
	{
	public:
		CModel* GetModel();
		void Init(CModel* aModel);
		void Init(CModel* aModel, CAnimator* anAnimator);
		void SetTransform(const Vector3f& aPosition, const Vector3f& aRotation);
		void SetTransform(const Matrix4x4f& aTransform);
		Matrix4x4f& GetTransform();
		void SetRotation(const Vector3f& aRotation);
		void SetPosition(const Vector3f& aPosition);
		const Vector3f GetRotation() const;
		const Vector3f GetPosition() const;
		void Move(const Vector3f& aMovement);
		void Rotate(const Vector3f& aRotation);

		void Update(float aDeltaTime);
		void Render();

		const Vector3f& GetScale()const noexcept;
		void SetScale(const Vector3f& aScale);

	private:
		CModel* myModel = nullptr;
		CAnimator* myAnimator = nullptr;
		CTransform myTransform;
		Vector3f myScale;
	};
}
