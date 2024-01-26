#include "pch.h"
#include "TriggerSystem.h"
#include "TriggerEvents.h"
#include "Coordinator.h"
#include "TriggerData.h"
#include "TriggerListener.h"
#include <Engine/Engine.h>
#include "ModelCollection.h"
#include <Engine\DX11.h>
#include <Engine\ContentLoader.h>

#include <Engine\CDebugDrawer.h>

void TriggerSystem::CreateTriggerEvent(const TriggerData& someData)
{
	myTriggerData.emplace_back(someData);
}

void TriggerSystem::RegisterTriggerListener(TriggerListener* const& aTriggerListener, const std::initializer_list<TriggerEvent>& someEvents)
{
	for (auto& event : someEvents)
	{
		RegisterTriggerListener(aTriggerListener, event);
	}
}

void TriggerSystem::RegisterTriggerListener(TriggerListener* const& aTriggerListener, TriggerEvent anEvent)
{
	myTriggerListeners[anEvent].insert(aTriggerListener);
}

void TriggerSystem::DistributeEvents()
{
	for (auto& data : myTriggerData)
	{
		for (auto& listener : myTriggerListeners.at(data.event))
		{
			listener->OnTriggerEnter(data);
		}
	}
	myTriggerData.clear();
}

#include <Engine\DX11.h>
#include <Engine\ContentLoader.h>
#include "ModelCollection.h"
Entity TriggerSystem::CreateTrigger(Trigger& aTrigger, Transform& aTransform)
{
	Entity entity = myCoordinator->CreateEntity();
	myCoordinator->AddComponent(entity, aTrigger);
	myCoordinator->AddComponent(entity, aTransform);

	return entity;
}

void TriggerSystem::DeleteDeadTriggers()
{
	float deltaTime = SE::CEngine::GetInstance()->GetDeltaTime();

	myDeadTriggerCount = 0;
	for (auto& entity : myEntities)
	{
		auto& trigger = myCoordinator->GetComponent<Trigger>(entity);

		if (trigger.shape.type == ShapeType::Sphere)
		{
			auto& transform = myCoordinator->GetComponent<Transform>(entity);
			CDebugDrawer::GetInstance().DrawSphere(transform.GetPosition(), trigger.shape.asSphere.radius, { 1, 0, 0, 1 });
		}
		if (trigger.shape.type == ShapeType::Cone)
		{
			auto& transform = myCoordinator->GetComponent<Transform>(entity);
			CDebugDrawer::GetInstance().DrawSphere(transform.GetPosition(), trigger.shape.asCone.radius, { 1, 0, 0, 1 });
		}

		if (trigger.lifeTime < .0f)
		{
			if (myDeadTriggers.size() == myDeadTriggerCount)
			{
				myDeadTriggers.emplace_back(0);
			}
			myDeadTriggers[myDeadTriggerCount++] = entity;
		}
		trigger.lifeTime -= deltaTime;
	}

	for (size_t i = 0; i < myDeadTriggerCount; i++)
	{
		Entity entity = myDeadTriggers[i];
		myCoordinator->DestroyEntity(entity);
	}
}
