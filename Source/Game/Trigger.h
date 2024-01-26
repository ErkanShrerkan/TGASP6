#pragma once
#include "Shape.h"
#include "TriggerEvents.h"

struct Trigger
{
    Shape shape;
    TriggerEvent event;

    float lifeTime = 1.0f / 100000.f;
    float damage;
};
