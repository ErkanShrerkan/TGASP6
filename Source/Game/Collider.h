#pragma once
#include "Shape.h"
#include "TriggerEvents.h"
#include <set>

struct Collider
{
    Shape shape;
    std::set<TriggerEvent> canCollideWithEvents;
};
