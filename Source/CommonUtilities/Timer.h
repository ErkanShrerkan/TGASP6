#pragma once
#include <iostream>
#include <chrono>

namespace CommonUtilities
{
	class Timer
	{
	public:
		Timer();

		Timer(const Timer& aTimer) = delete;
		Timer& operator=(const Timer& aTimer) = delete;

		void Update();

		float GetDeltaTime() const;
		double GetTotalTime() const;

	private:
		std::chrono::steady_clock::time_point myTimePointStart;
		std::chrono::steady_clock::time_point myTimePointBeginFrame;
		std::chrono::steady_clock::time_point myTimePointEndFrame;
		float myDeltaTime;
	};
}
