#include "pch.h"
#include "Timer.h"

namespace CommonUtilities
{
	Timer::Timer()
	{
		myTimePointStart = std::chrono::high_resolution_clock::now();
		myTimePointBeginFrame = std::chrono::high_resolution_clock::now();
		myTimePointEndFrame = std::chrono::high_resolution_clock::now();
		myDeltaTime = 0;
	}

	void Timer::Update()
	{
		myTimePointEndFrame = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> deltaTime = myTimePointEndFrame - myTimePointBeginFrame;
		myTimePointBeginFrame = myTimePointEndFrame;
		myDeltaTime = deltaTime.count();
	}

	float Timer::GetDeltaTime() const
	{
		return myDeltaTime;
	}

	double Timer::GetTotalTime() const
	{
		std::chrono::duration<double> deltaTime = std::chrono::high_resolution_clock::now() - myTimePointStart;
		double totalTime = deltaTime.count();
		return totalTime;
	}
}