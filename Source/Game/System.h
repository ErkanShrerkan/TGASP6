#pragma once
#include "ECSSettings.h"
#include <set>

class Coordinator;
template <class... ComponentTypes>
class System
{
private:
    friend class SystemCoupler;
    Signature mySignature;

protected:
    std::set<Entity> myEntities;
    Coordinator* myCoordinator;
    // TODO: Maybe add GetComponent<Component>
    // for easier access instead of manually
    // getting the ECS::Coordinater everytime

public:
    virtual void EntityGotDestroyed(Entity) {};
    virtual ~System() = default;
    System()
    {
        ComponentID cids[] = { 0, ECS::GetComponentID<ComponentTypes>()... };
        Signature signature = 0;
        for (size_t i = 1; i < sizeof...(ComponentTypes) + 1; i++)
        {
            signature.set(cids[i], true);
        }
        mySignature = signature;
    }
};
