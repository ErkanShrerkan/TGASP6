#include "pch.h"
#include "CollisionSystem.h"
#include "Coordinator.h"
#include "TriggerData.h"
#include "TriggerSystem.h"
#include <Engine\CDebugDrawer.h>

namespace SE
{
	class CModel;
}

void CollisionSystem::CheckCollisions(TriggerSystem& aTriggerSystem)
{

#ifdef _DEBUG
	for (auto& entity : myEntities)
	{
		auto& transform = myCoordinator->GetComponent<Transform>(entity);
		auto& collider = myCoordinator->GetComponent<Collider>(entity);

		if (collider.shape.type == ShapeType::Sphere)
			CDebugDrawer::GetInstance().DrawSphere(transform.GetPosition(), collider.shape.asSphere.radius, { 0, 1, 0, 1 });

		if (collider.shape.type == ShapeType::Cone)
			CDebugDrawer::GetInstance().DrawSphere(transform.GetPosition(), collider.shape.asCone.radius, { 1, 1, 0, 1 });
	}
#endif // _DEBUG

	for (auto& entityT : aTriggerSystem.myEntities)
	{
		auto& transformT = myCoordinator->GetComponent<Transform>(entityT);
		auto& trigger = myCoordinator->GetComponent<Trigger>(entityT);

		for (auto& entityC : myEntities)
		{
			auto& transformC = myCoordinator->GetComponent<Transform>(entityC);
			auto& collider = myCoordinator->GetComponent<Collider>(entityC);

			if (collider.canCollideWithEvents.find(trigger.event) != collider.canCollideWithEvents.end())
			{
				switch (trigger.shape.type)
				{
				case ShapeType::Cone:
					if (IsConeTouchingSphere(trigger.shape, transformT, transformC))
					{
						TriggerData data;
						data.affectedEntity = entityC;
						data.event = trigger.event;
						data.damage = trigger.damage;
						aTriggerSystem.CreateTriggerEvent(data);
					}
					break;
				case ShapeType::Sphere:
					if (IsSphereTouchingSphere(trigger.shape, transformT, collider.shape, transformC))
					{
						TriggerData data;
						data.affectedEntity = entityC;
						data.event = trigger.event;
						data.damage = trigger.damage;
						aTriggerSystem.CreateTriggerEvent(data);
					}
					break;
				default: __assume(0);
				}
			}
		}
	}
}

bool CollisionSystem::IsSphereTouchingSphere(
	Shape& aShapeA, Transform& aTransformA,
	Shape& aShapeB, Transform& aTransformB
) const
{
	float distanceX = aTransformA.GetTransform()(4, 1) - aTransformB.GetTransform()(4, 1); // x
	float distanceY = aTransformA.GetTransform()(4, 2) - aTransformB.GetTransform()(4, 2); // y
	float distanceZ = aTransformA.GetTransform()(4, 3) - aTransformB.GetTransform()(4, 3); // z
	
	float distanceSquared = 0.0f;
	distanceSquared += distanceX * distanceX;
	distanceSquared += distanceY * distanceY;
	distanceSquared += distanceZ * distanceZ;

	float squaredRadius = aShapeA.asSphere.radius + aShapeB.asSphere.radius;
	squaredRadius *= squaredRadius;

	return distanceSquared < squaredRadius;
}


#include <Engine\DX11.h>
#include <Engine\ContentLoader.h>
#include "ModelCollection.h"

bool CollisionSystem::IsConeTouchingSphere(Shape& aShapeA, Transform& aTransformA, Transform& aTransformB) const
{
	float length = aShapeA.asCone.radius;
	float angle = aShapeA.asCone.angle / 2.f;


	float2 dir = { aTransformB.GetPosition().x - aTransformA.GetPosition().x, aTransformB.GetPosition().z - aTransformA.GetPosition().z };

	float2 re = *reinterpret_cast<float2*>(&aShapeA.asCone.x);

	re.Normalize();

	float radian = acosf(dir.GetNormalized().Dot(re));
	radian = Math::RadianToDegree(radian);

	return (radian <= angle) && (dir.Length() <= length);

}

bool CollisionSystem::IsClockwise(Vector2f aVec1, Vector2f aVec2) const
{
	return -aVec1.x * aVec2.y + aVec1.y * aVec2.x > 0;
}

bool CollisionSystem::IsWithinRadius(Vector2f aVec, float aRadSqr) const
{
	return aVec.x * aVec.x + aVec.y * aVec.y <= aRadSqr;
}

