#include "pch.h"
#include "Coordinator.h"

Entity Coordinator::CreateEntity()
{
    return myEntityManager.CreateEntity();
}

void Coordinator::DestroyEntity(Entity anEntity)
{
    myEntityManager.DestroyEntity(anEntity);

    myComponentManager.EntityGotDestroyed(anEntity);

    mySystemCoupler.EntityGotDestroyed(anEntity);
}
