#include "pch.h"
#include "Stopwatch.h"

Stopwatch::Stopwatch(float aTimeInterval)
	: myTime(0), myTimeInterval(aTimeInterval), myState(eWatchState::Under)
{
}

Stopwatch::Stopwatch() : myTime(0), myTimeInterval(1.0f / 15), myState(eWatchState::Under)
{
}

Stopwatch::~Stopwatch()
{
}

float Stopwatch::GetTime()
{
	return myTime;
}

float Stopwatch::GetTimeInterval()
{
	return myTimeInterval;
}

void Stopwatch::SetTimeInterval(float aTimeInterval)
{
	myTimeInterval = aTimeInterval;
}

void Stopwatch::Reset()
{
	myTime = 0;
}

void Stopwatch::Update(float aDeltaTime, bool myReset)
{
	myTime += aDeltaTime;
	myTime = myTime < 0 ? 0 : myTime;

	if (!myReset) 
	{
		myTime = myTime > myTimeInterval ? myTimeInterval : myTime;
	}

	if (myTime >= myTimeInterval)
	{
		myState = eWatchState::Over;
		if (myReset)
		{
			Reset();
		}
	}
	else myState = eWatchState::Under;
}

void Stopwatch::SetIsOver()
{
	myTime = myTimeInterval;
	myState = eWatchState::Over;
}

Stopwatch::eWatchState Stopwatch::GetState()
{
	return myState;
}

bool Stopwatch::IsOver()
{
	return (myState == eWatchState::Over);
}