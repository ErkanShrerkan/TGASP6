#include "pch.h"
#include "LightFactory.h"
#include "EnvironmentLight.h"
#include "PointLight.h"
#include "SpotLight.h"

// To get Cubemap
#include "Texture.h"
#include "TextureHelper.h"
#include "Camera.h"
#include "Engine.h"
#include "ContentLoader.h"

/*
 * Good settings for point lights:
 * ---------------{  X    Y    Z    W  } {   R     G     B    A  }
 * Red   : Fire   { 114, 144, -92, 160 } { .905, .105, .005,  50 }
 * Green : Goblet { -56,  21, -69,  53 } { .005, .265, .035,   2 }
 * Blue  : Magic  {   0,   0,  57,  84 } { .050, .025, .900,   4 }
 */

namespace SE
{
	CEnvironmentLight* CLightFactory::CreateEnvironmentLight(const std::string_view& aPath)
	{
		CEnvironmentLight* light = new CEnvironmentLight();

		CTexture texture = CTexture(aPath);
		light->myCubemap = texture.StealShaderResource();
		light->myMipLevels = texture.GetMipLevels();

		std::shared_ptr<CCamera> cam = std::make_shared<CCamera>();
		cam->Init({ 2750, 2750 });
		light->AddProjection(cam->GetProjection());
		cam->Init({ 2750 * 3, 2750 * 3});
		light->AddProjection(cam->GetProjection());
		light->myShadowCam = cam;

		return light;
	}
	CPointLight* CLightFactory::CreatePointLight()
	{
		CPointLight* light = new CPointLight();

		return light;
	}
	CSpotLight* CLightFactory::CreateSpotLight()
	{
		CSpotLight* light = new CSpotLight();

		return light;
	}
}
