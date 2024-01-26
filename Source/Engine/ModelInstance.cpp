#include "pch.h"
#include "ModelInstance.h"
#include "Scene.h"
#include "Engine.h"
#include "Animator.h"

namespace SE
{
	CModel* CModelInstance::GetModel()
	{
		return myModel;
	}

	void CModelInstance::Init(CModel* aModel)
	{
		myModel = aModel;
		myScale = Vector3f(1.f, 1.f, 1.f);
	}

	void CModelInstance::Init(CModel* aModel, CAnimator* anAnimator)
	{
		myModel = aModel;
		myScale = Vector3f(1.f, 1.f, 1.f);
		myAnimator = anAnimator;
	}

	void CModelInstance::SetTransform(const Vector3f& aPosition, const Vector3f& aRotation)
	{
		myTransform.SetTransform(aPosition, aRotation);
	}

	void CModelInstance::SetTransform(const Matrix4x4f& aTransform)
	{
		myTransform.SetTransform(aTransform);
	}

	Matrix4x4f& CModelInstance::GetTransform()
	{
		return myTransform.GetTransform();
	}

	void CModelInstance::SetRotation(const Vector3f& aRotation)
	{
		myTransform.SetRotation(aRotation);
	}

	void CModelInstance::SetPosition(const Vector3f& aPosition)
	{
		myTransform.SetPosition(aPosition);
	}

	const Vector3f CModelInstance::GetRotation() const
	{
		return myTransform.GetRotation();
	}

	const Vector3f CModelInstance::GetPosition() const
	{
		return myTransform.GetPosition();
	}

	void CModelInstance::Move(const Vector3f& aMovement)
	{
		myTransform.Move(aMovement);
	}

	void CModelInstance::Rotate(const Vector3f& aRotation)
	{
		myTransform.Rotate(aRotation);
	}

	void CModelInstance::Update(float aDeltaTime)
	{
		if (myAnimator)
		{
			myAnimator->Update(aDeltaTime);
		}
	}

	void CModelInstance::Render()
	{
		//broken
		ModelCollection collection{ myModel, nullptr };
		SE::CEngine::GetInstance()->GetActiveScene()->AddInstance(&collection, myTransform.GetTransform(), myScale);
	}

	const Vector3f& CModelInstance::GetScale() const noexcept
	{
		return myScale;
	}

	void CModelInstance::SetScale(const Vector3f& aScale)
	{
		myScale = aScale;
	}
}
