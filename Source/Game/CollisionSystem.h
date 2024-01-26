#pragma once
#include <set>
#include "System.h"
#include "Collider.h"
#include "Transform.h"

class TriggerSystem;
class CollisionSystem : public System<Collider, Transform>
{
public:
    void CheckCollisions(TriggerSystem& aTriggerSystem);

private:
    bool IsSphereTouchingSphere(
        Shape& aShapeA, Transform& aTransformA,
        Shape& aShapeB, Transform& aTransformB
    ) const;
    bool IsConeTouchingSphere(
        Shape& aShapeA, Transform& aTransformA,
        Transform& aTransformB
    ) const;

private:
    bool IsClockwise(Vector2f aVec1, Vector2f aVec2) const;
    bool IsWithinRadius(Vector2f aVec, float aRadSqr) const;

};
