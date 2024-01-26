#pragma once
#include <string_view>
namespace SE
{
	class CEnvironmentLight; // CDirectionalLight???
	class CPointLight;
	class CSpotLight;
	class CLightFactory
	{
	public:
		CEnvironmentLight* CreateEnvironmentLight(const std::string_view& aPath);
		CPointLight* CreatePointLight();
		CSpotLight* CreateSpotLight();
	};
}
