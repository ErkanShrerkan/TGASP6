#pragma once
#include <string>
#include <vector>

template <class Instance>
Instance& Singleton()
{
	static Instance instance;
	return instance;
}
