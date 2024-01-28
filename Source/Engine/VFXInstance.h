#pragma once

namespace SE
{
	class CVFX;
	class CVFXInstance
	{
		friend class CVFXRenderer;
		friend class CVFXManager;

	private:
		CVFX* myVFX = nullptr;
		bool myIsDone = false;
		bool myLoop = false;
		float myTime = 0;

		std::vector<float4> myColors;
		std::vector<float3> myScales;
		std::vector<float2> myUVs;
		std::vector<int> myBlends;
		std::vector<bool> myDeadComponents;

		CVFXInstance(CVFX* aVFX);
		CVFXInstance() = delete;

		void Update(float aDeltaTime);
		bool IsDone() { return myIsDone; }
	};

}
