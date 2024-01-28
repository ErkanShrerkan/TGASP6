#include "pch.h"
#include "VFXManager.h"
#include "VFXInstance.h"
#include "VFXFactory.h"
#include "VFX.h"

namespace SE
{

	void CVFXManager::PlayVFX(std::string aVFX, const SE::CTransform& aTransform)
	{
		SVFXCollection vfxc;
		vfxc.transform = aTransform;
		vfxc.vfx = new CVFXInstance(CVFXFactory::GetInstance().GetVFX(aVFX));
		myActiveVFXs.push_back(vfxc);
	}

	void CVFXManager::ClearVFXs()
	{
		for (auto& vfxc : myActiveVFXs)
		{
			delete vfxc.vfx;
		}

		myActiveVFXs.clear();
	}

	void CVFXManager::Update(float aDeltaTime)
	{
		for (auto& vfx : myActiveVFXs)
		{
			if (!vfx.vfx->IsDone())
			{
				vfx.vfx->Update(aDeltaTime);
			}
			else
			{
				delete vfx.vfx;
				vfx.vfx = nullptr;
			}
		}

		myActiveVFXs.erase(std::remove_if(myActiveVFXs.begin(), myActiveVFXs.end(), [&](SVFXCollection& vfxc) { return !vfxc.vfx; }), myActiveVFXs.end());
	}

}