#pragma once
#include "eMessageTypes.h"
class Observer
{
public:
	virtual void RecieveMessage(eMessage aMsg) = 0;
};

