#pragma once
#include <unordered_map>

namespace SE
{
	class CVFX;
	class CVFXFactory
	{
	public:
		static CVFXFactory& GetInstance()
		{
			static CVFXFactory instance;
			return instance;
		}

		CVFX* GetVFX(std::string aVFX);

		CVFXFactory(const CVFXFactory& aFactory) = delete;
		void operator=(const CVFXFactory& aFactory) = delete;

	private:
		CVFX* CreateVFX(std::string aVFX);
		CVFX* LoadVFX(std::string aVFX);

		std::unordered_map<std::string, CVFX*> myVFXs;

		CVFXFactory() {}
	};

}
