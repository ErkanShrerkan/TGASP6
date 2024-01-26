#include "pch.h"
#include "Component.h"

namespace SE
{
    CComponent::CComponent(CGameObject& aGameObject)
        : myParent(aGameObject)
    {
    }

    CGameObject& CComponent::GetParent() const
    {
        return myParent;
    }
}
