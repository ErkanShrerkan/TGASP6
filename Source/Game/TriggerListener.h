#pragma once
#include "TriggerData.h"

class __declspec(novtable) TriggerListener
{
    friend class TriggerSystem;
protected:
    virtual void OnTriggerEnter(const TriggerData& someTriggerData) = 0;
};
