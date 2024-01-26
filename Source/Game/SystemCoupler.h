#pragma once
#include "ECSSettings.h"
#include <unordered_map>
#include <typeinfo>
#include "System.h"

class Coordinator;
class SystemCoupler
{
private:
    std::unordered_map<SystemID, Signature> mySignatures;
    std::unordered_map<SystemID, System<>*> mySystems;

public:
    template <class... ComponentTypes>
    void AddSystem(Coordinator* const& aCoordinator, System<ComponentTypes...>& aSystem)
    {
        SystemID sid = ECS::GetSystemID<System<ComponentTypes...>>();
        System<>* system = reinterpret_cast<System<>*>(&aSystem);

        system->myCoordinator = aCoordinator;
        mySystems[sid] = system;
        mySignatures[sid] = system->mySignature;
    }
    void EntitySignatureChanged(Entity anEntity, Signature aSignature);
    void EntityGotDestroyed(Entity anEntity);
};
