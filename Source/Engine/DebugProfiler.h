#pragma once
#include <chrono>
#include <queue>
#include <CommonUtilities\Container\CSFifoArray.hpp>
#include "Texture.h"
namespace SE
{
	namespace Debug
	{
		class CDebugProfiler
		{
		public:
			CDebugProfiler();
			void BeginCapture();
			void EndCapture();
			void Render();

			void IncrementDrawCallCount();

		private:
			int myAverageFPS		= 0;
			int myMemoryUsage		= 0;

			int myOldMallocs		= 0;
			int myMallocDiff		= 0;

			int myDrawCallCount		= 0;
			int myDrawCallCountSum  = 0;

			float myTimer			= 0.f;
			float myAverageTime		= 0.f;
			float myTotalFrameTime	= 0.f;
			float myFrameCounter	= 0.f;
			float myRefreshRate		= 0.25f;
			CommonUtilities::CStyle::FIFOArray<float, 60> myAllocations;
			CommonUtilities::CStyle::FIFOArray<float, 60> myFrameRates;
			CommonUtilities::CStyle::FIFOArray<float, 60> myFrameDrops;
			CommonUtilities::CStyle::FIFOArray<float, 60> myAverageTimes;
			CommonUtilities::CStyle::FIFOArray<float, 60> myAverageDrawCalls;

			std::chrono::high_resolution_clock myClock;
			std::chrono::high_resolution_clock::time_point myFirstTime;

			CTexture texture;
		};
	}
}
