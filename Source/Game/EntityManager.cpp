#include "pch.h"
#include "EntityManager.h"

EntityManager::EntityManager()
{
    for (Entity entity = 0; entity < MAX_ENTITIES; entity++)
    {
        myAvailableEntities.push(entity);
    }
}

Entity EntityManager::CreateEntity()
{
    // Get available entity
    Entity entity = myAvailableEntities.front();
    myAvailableEntities.pop();
    ++myLivingEntityCount;

    return entity;
}

void EntityManager::DestroyEntity(Entity anEntity)
{
    // Invalidate all components
    mySignatures[anEntity].reset();

    // Put the destroyed entity at the back of the queue
    myAvailableEntities.push(anEntity);
    --myLivingEntityCount;
}

void EntityManager::SetSignature(Entity entity, Signature signature)
{
    mySignatures[entity] = signature;
}

Signature EntityManager::GetSignature(Entity entity)
{
    return mySignatures[entity];
}
