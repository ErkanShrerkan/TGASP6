#pragma once
class Stopwatch
{
public:
	enum class eWatchState
	{
		Under,
		Over
	};

	Stopwatch(float aTimeInterval);
	Stopwatch();
	~Stopwatch();

	float GetTime();
	float GetTimeInterval();
	void SetTimeInterval(float aTimeInterval);
	void Reset();
	void Update(float aDeltaTime, bool myReset = true);
	void SetIsOver();
	eWatchState GetState();
	bool IsOver();

private:
	float myTime;
	float myTimeInterval;
	eWatchState myState;
};