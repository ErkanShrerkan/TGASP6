#include "pch.h"
#include "SystemCoupler.h"
#include "System.h"

void SystemCoupler::EntitySignatureChanged(Entity anEntity, Signature aSignature)
{
    for (const auto& pair : mySystems)
    {
        const auto& systemId = pair.first;
        const auto& system = pair.second;
        const auto& signature = mySignatures.at(systemId);

        // Check if the entity still has all the needed
        // components for this system to run update on it
        if ((aSignature & signature) == signature)
        {
            // Has all needed components
            system->myEntities.insert(anEntity);
        }
        else
        {
            // Lacks needed components
            system->myEntities.erase(anEntity);
        }
    }
}

void SystemCoupler::EntityGotDestroyed(Entity anEntity)
{
    for (const auto& pair : mySystems)
    {
        const auto& system = pair.second;

        system->EntityGotDestroyed(anEntity);
        system->myEntities.erase(anEntity);
    }
}
