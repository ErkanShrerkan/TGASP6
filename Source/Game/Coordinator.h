#pragma once
#include "ECSSettings.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemCoupler.h"

class Coordinator
{
private:
    EntityManager myEntityManager;
    ComponentManager myComponentManager;
    SystemCoupler mySystemCoupler;

public:
#pragma region Entity Region

    Entity CreateEntity();
    void DestroyEntity(Entity anEntity);

#pragma endregion
#pragma region Component Region

    template <class Component>
    void RegisterComponentArray(ComponentArray<Component>& anArray)
    {
        myComponentManager.AddComponentArray(anArray);
    }
    template <class Component>
    void AddComponent(Entity anEntity, const Component& aComponent)
    {
        myComponentManager.AddComponent(anEntity, aComponent);

        Signature signature = myEntityManager.GetSignature(anEntity);
        ComponentID cid = ECS::GetComponentID<Component>();
        (void)cid;
        signature.set(ECS::GetComponentID<Component>(), true);
        //signature |= 1u << ECS::GetComponentID<Component>();
        myEntityManager.SetSignature(anEntity, signature);

        mySystemCoupler.EntitySignatureChanged(anEntity, signature);
    }
    template <class Component>
    void RemoveComponent(Entity anEntity)
    {
        myComponentManager.RemoveComponent<Component>(anEntity);

        Signature signature = myEntityManager.GetSignature(anEntity);
        signature.set(ECS::GetComponentID<Component>(), false);
        //signature ^= 1u << ECS::GetComponentID<Component>();
        myEntityManager.SetSignature(anEntity, signature);

        mySystemCoupler.EntitySignatureChanged(anEntity, signature);
    }
    template <class Component>
    Component& GetComponent(Entity anEntity)
    {
        return myComponentManager.GetComponent<Component>(anEntity);
    }

#pragma endregion
#pragma region System Region

    template <class... ComponentTypes>
    void RegisterSystem(System<ComponentTypes...>& aSystem)
    {
        mySystemCoupler.AddSystem(this, aSystem);
    }

#pragma endregion
};
