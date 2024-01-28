#include "pch.h"
#include "VFXInstance.h"
#include "VFX.h"
#include <Game\macro.h>

namespace SE
{
	void CVFXInstance::Release()
	{
		delete myVFX;
		myVFX = nullptr;
	}

	CVFXInstance::CVFXInstance(CVFX* aVFX)
		: myVFX(aVFX)
	{
		int size = (int)aVFX->myData.size();
		myColors.resize(size);
		myScales.resize(size);
		myUVs.resize(size);
		myBlends.resize(size);
		myDeadComponents.resize(size);
		myLoop = aVFX->myLooping;
	}

	void CVFXInstance::Update(float aDeltaTime)
	{
		myTime += aDeltaTime;

		for (int component = 0; component < myVFX->myData.size(); component++)
		{
			auto& data = myVFX->myData[component];

			if (myDeadComponents[component])
			{
				continue;
			}

			float4 color;
			float3 scale;
			float2 uv;

			float frame = myTime * data.fps;

			if (frame > data.frames)
			{
				//myVFX->myData[component].model = nullptr;
				myDeadComponents[component] = true;
				continue;
			}

			for (int i = 0; i < data.colorKeys.size(); i++)
			{
				if (data.colorKeys.size() == 1)
				{
					memcpy(&color.r, &data.colorKeys[i].r, sizeof(float) * 4);
					break;
				}

				if (data.colorKeys[i].frame > frame)
				{
					float4 lastCol = *reinterpret_cast<float4*>(&data.colorKeys[i - 1].r);
					float4 nextCol = *reinterpret_cast<float4*>(&data.colorKeys[i].r);
					float lastFrame = data.colorKeys[i - 1].frame;
					float nextFrame = data.colorKeys[i].frame;
					float lerpValue = (frame - lastFrame) / (nextFrame - lastFrame);
					color.r = Math::Lerp(lastCol.r, nextCol.r, lerpValue);
					color.g = Math::Lerp(lastCol.g, nextCol.g, lerpValue);
					color.b = Math::Lerp(lastCol.b, nextCol.b, lerpValue);
					color.a = Math::Lerp(lastCol.a, nextCol.a, lerpValue);
					break;
				}
			}

			for (int i = 0; i < data.scaleKeys.size(); i++)
			{
				if (data.scaleKeys.size() == 1)
				{
					memcpy(&scale.x, &data.scaleKeys[i].x, sizeof(float) * 3);
					break;
				}

				if (data.scaleKeys[i].frame > frame)
				{
					float3 lastScl = *reinterpret_cast<float3*>(&data.scaleKeys[i - 1].x);
					float3 nextScl = *reinterpret_cast<float3*>(&data.scaleKeys[i].x);
					float lastFrame = data.scaleKeys[i - 1].frame;
					float nextFrame = data.scaleKeys[i].frame;
					float lerpValue = (frame - lastFrame) / (nextFrame - lastFrame);
					scale = Math::Lerp(lastScl, nextScl, lerpValue);
					break;
				}
			}

			for (int i = 0; i < data.uvKeys.size(); i++)
			{
				if (data.scaleKeys.size() == 1)
				{
					memcpy(&uv.x, &data.uvKeys[i].u, sizeof(float) * 2);
					break;
				}

				if (data.uvKeys[i].frame > frame)
				{
					float2 lastScl = *reinterpret_cast<float2*>(&data.uvKeys[i - 1].u);
					float2 nextScl = *reinterpret_cast<float2*>(&data.uvKeys[i].u);
					float lastFrame = data.uvKeys[i - 1].frame;
					float nextFrame = data.uvKeys[i].frame;
					float lerpValue = (frame - lastFrame) / (nextFrame - lastFrame);
					uv = Math::Lerp(lastScl, nextScl, lerpValue);
					break;
				}
			}

			color /= 255.f;
			myColors[component] = color;
			myScales[component] = scale;
			myUVs[component] = uv;
		}

		int alldead = 1;
		for (int i = 0; i < myDeadComponents.size(); i++)
		{
			alldead *= myDeadComponents[i];
		}

		if (alldead)
		{
			myIsDone = true;

			if (myLoop)
			{
				myIsDone = false;
				myTime = 0;
				for (int i = 0; i < myDeadComponents.size(); i++)
				{
					myDeadComponents[i] = false;
				}
			}
		}
	}
}