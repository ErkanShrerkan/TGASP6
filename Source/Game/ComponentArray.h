#pragma once
#include "ECSSettings.h"
#include <array>
#include <unordered_map>

class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void EntityGotDestroyed(Entity anEntity) = 0;
};

template <class Component>
class ComponentArray : public IComponentArray
{
private:
	std::array<Component, MAX_ENTITIES> myComponentArray;
	std::unordered_map<Entity, EntityType> myEntityToIndexMap;
	std::unordered_map<EntityType, Entity> myIndexToEntityMap;
	EntityType myComponentsCount = 0;

public:
	void AddComponent(Entity anEntity, const Component& aComponent)
	{
		EntityType newIndex = myComponentsCount;
		myEntityToIndexMap[anEntity] = newIndex;
		myIndexToEntityMap[newIndex] = anEntity;
		myComponentArray[newIndex] = aComponent;
		++myComponentsCount;
	}
	void RemoveComponent(Entity anEntity)
	{
		EntityType indexOfRemovedEntity = myEntityToIndexMap[anEntity];
		EntityType indexOfLastElement = myComponentsCount - 1u;
		// do da switcheroo
		myComponentArray[indexOfRemovedEntity] = myComponentArray[indexOfLastElement];

		// Update map to point correctly
		Entity entityOfLastIndex = myIndexToEntityMap[indexOfLastElement];
		myEntityToIndexMap[entityOfLastIndex] = indexOfRemovedEntity;
		myIndexToEntityMap[indexOfRemovedEntity] = entityOfLastIndex;

		myEntityToIndexMap.erase(anEntity);
		myIndexToEntityMap.erase(indexOfLastElement);
		--myComponentsCount;
	}
	Component& GetComponent(Entity anEntity)
	{
		return myComponentArray[myEntityToIndexMap[anEntity]];
	}
	void EntityGotDestroyed(Entity anEntity) override
	{
		// Check if entity has component
		if (myEntityToIndexMap.find(anEntity) != myEntityToIndexMap.end())
		{
			RemoveComponent(anEntity);
		}
	}
};
