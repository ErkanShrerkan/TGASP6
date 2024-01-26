#pragma once
#include "ECSSettings.h"
#include <unordered_map>
#include "ComponentArray.h"

// ComponentShephard ???
class ComponentManager
{
private:
    std::unordered_map<ComponentID, IComponentArray*> myComponentArrays;
    ComponentID myCurrentComponentType;

private:
    template <class Component>
    ComponentArray<Component>* GetComponentArray()
    {
        return reinterpret_cast<ComponentArray<Component>*>(myComponentArrays[ECS::GetComponentID<Component>()]);
    }

public:
    template <class Component>
    void AddComponentArray(ComponentArray<Component>& anArray)
    {
        ComponentID componentID = ECS::GetComponentID<Component>();
        myComponentArrays.insert({ componentID, &anArray });

        ++myCurrentComponentType;
    }
    template <class Component>
    void AddComponent(Entity anEntity, const Component& aComponent)
    {
        GetComponentArray<Component>()->AddComponent(anEntity, aComponent);
    }
    template <class Component>
    void RemoveComponent(Entity anEntity)
    {
        GetComponentArray<Component>()->RemoveComponent(anEntity);
    }
    template <class Component>
    Component& GetComponent(Entity anEntity)
    {
        return GetComponentArray<Component>()->GetComponent(anEntity);
    }
    void EntityGotDestroyed(Entity anEntity)
    {
        for (const auto& pair : myComponentArrays)
        {
            const auto& componentArray = pair.second;
            componentArray->EntityGotDestroyed(anEntity);
        }
    }
};
