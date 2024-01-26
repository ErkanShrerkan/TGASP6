#pragma once
#include "ECSSettings.h"
#include "TriggerEvents.h"

struct TriggerData
{
    Entity affectedEntity;
    TriggerEvent event;
    float damage;
};
