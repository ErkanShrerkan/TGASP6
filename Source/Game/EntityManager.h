#pragma once
#include "ECSSettings.h"
#include <queue>
#include <array>

class EntityManager
{
private:
    std::queue<Entity> myAvailableEntities;
    std::array<Signature, MAX_ENTITIES> mySignatures;
    EntityType myLivingEntityCount = 0;

public:
    EntityManager();
    Entity CreateEntity();
    void DestroyEntity(Entity anEntity);
    void SetSignature(Entity entity, Signature signature);
    Signature GetSignature(Entity entity);
};
