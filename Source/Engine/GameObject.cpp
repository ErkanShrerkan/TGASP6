#include "pch.h"
#include "GameObject.h"
#include "Component.h"
namespace SE
{
    void CGameObject::Init()
    {
        for (auto& componentVector : myComponents)
        {
            for (CComponent*& component : componentVector.second)
            {
                component->Init();
            }
        }
    }

    void CGameObject::Update()
    {
        for (auto& componentVector : myComponents)
        {
            for (CComponent*& component : componentVector.second)
            {
                component->Update();
            }
        }
    }

    bool CGameObject::IsMarkedForRemoval() const
    {
        return myIsMarkedForRemoval;
    }

    void CGameObject::MarkForRemoval()
    {
        myIsMarkedForRemoval = true;
    }

    void CGameObject::SetTransform(const Vector3f& aPosition, const Vector3f& aRotation)
    {
        myTransform.SetTransform(aPosition, aRotation);
    }

    void CGameObject::SetTransform(const Matrix4x4f& aTransform)
    {
        myTransform.SetTransform(aTransform);
    }

    Matrix4x4f& CGameObject::GetTransform()
    {
        return myTransform.GetTransform();
    }

    void CGameObject::SetRotation(const Vector3f& aRotation)
    {
        myTransform.SetRotation(aRotation);
    }

    void CGameObject::SetPosition(const Vector3f& aPosition)
    {
        myTransform.SetPosition(aPosition);
    }

    const Vector3f CGameObject::GetRotation() const
    {
        return myTransform.GetRotation();
    }

    const Vector3f CGameObject::GetPosition() const
    {
        return myTransform.GetPosition();
    }

    void CGameObject::Move(const Vector3f& aMovement)
    {
        myTransform.Move(aMovement);
    }

    void CGameObject::Rotate(const Vector3f& aRotation)
    {
        myTransform.Rotate(aRotation);
    }

    const Vector3f& CGameObject::GetScale() const noexcept
    {
        return myScale;
    }

    void CGameObject::SetScale(const Vector3f& aScale)
    {
        myScale = aScale;
    }
}
