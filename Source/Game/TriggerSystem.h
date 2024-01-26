#pragma once
#include "System.h"
#include "Trigger.h"
#include "Transform.h"
#include <set>
#include <vector>
#include <unordered_map>

struct TriggerData;
class TriggerListener;
class TriggerSystem : public System<Trigger, Transform>
{
    friend class CollisionSystem;
private:
    std::vector<TriggerData> myTriggerData; // <- plural form of Data
    std::unordered_map<TriggerEvent, std::set<TriggerListener*>> myTriggerListeners;

    std::vector<Entity> myDeadTriggers;
    size_t myDeadTriggerCount = 0;

private:
    void CreateTriggerEvent(const TriggerData& someData);

public:
    void RegisterTriggerListener(TriggerListener* const& aTriggerListener, TriggerEvent anEvent);
    void RegisterTriggerListener(TriggerListener* const& aTriggerListener, const std::initializer_list<TriggerEvent>& someEvents);
    void DistributeEvents();

    Entity CreateTrigger(Trigger& aTrigger, Transform& aTransform);
    void DeleteDeadTriggers();
};
