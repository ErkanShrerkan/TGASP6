#pragma once
#include <vector>
#include "Transform.h"

namespace SE
{

	class CVFXInstance;
	class CVFXManager
	{
		friend class CVFXRenderer;
	public:
		struct SVFXCollection
		{
			SE::CTransform transform;
			CVFXInstance* vfx = nullptr;
		};

		std::vector<CVFXManager::SVFXCollection>& GetVFXs() { return myActiveVFXs; }
		void PlayVFX(std::string aVFX, const SE::CTransform& aTransform);
		void ClearVFXs();
		void Update(float aDeltaTime);

		static CVFXManager& GetInstance()
		{
			static CVFXManager instance;
			return instance;
		}

		CVFXManager(const CVFXManager& aFactory) = delete;
		void operator=(const CVFXManager& aFactory) = delete;

	private:
		CVFXManager() {}

	private:

		std::vector<SVFXCollection> myActiveVFXs;
	};

}
