#pragma once
#include <memory>
// Factories
#include "ModelFactory.h"
#include "CameraFactory.h"
#include "LightFactory.h"
#include "TextureFactory.h"
#include "SpriteFactory.h"
#include "NavMeshLoader.h"
#include "ParticleFactory.h"
namespace SE
{
	// TODO: Remove overloaded load functions and
	// replace them with "GetXXXFactory()" instead

	class CContentLoader
	{
	public:

		// Use an overloaded version of this to load desired resource
		template <class Resource, class ...Args>
		Resource* Load(Args... someArguments);

		CFullscreenTexture Load(const Vector2ui& aSize, DXGI_FORMAT aFormat);
		CFullscreenTexture Load(ID3D11Texture2D* aTexture);

		SE::CModelFactory& GetModelFactory();
		SE::CCameraFactory& GetCameraFactory();
		SE::CLightFactory& GetLightFactory();
		SE::CTextureFactory& GetTextureFactory();
		SE::CSpriteFactory& GetSpriteFactory();
		SE::CNavMeshLoader& GetNavMeshLoader();
		SE::CParticleFactory& GetParticleFactory();

	private:
		SE::CModelFactory myModelFactory;
		SE::CCameraFactory myCameraFactory;
		SE::CLightFactory myLightFactory;
		SE::CTextureFactory myTextureFactory;
		SE::CSpriteFactory mySpriteFactory;
		SE::CNavMeshLoader myNavMeshLoader;
		SE::CParticleFactory myParticleFactory;
	};

	template<class Resource, class ...Args>
	inline Resource* CContentLoader::Load(Args ...someArguments)
	{
		//static_assert(false, "SE::CContentLoader<Resource>::Load() was called with an unsupported type or unsupported arguments");
		return nullptr;
	}

#define _MSE_LOAD_CONTENT(Type, ...) template <> inline Type* CContentLoader::Load<Type>(__VA_ARGS__)

	// TODO: Add description
	_MSE_LOAD_CONTENT(SE::CModel, const char* aPath)
	{
		return myModelFactory.GetModel(aPath);
	}
	// TODO: Add description
	_MSE_LOAD_CONTENT(SE::CModel, SE::EPrimitive aPrimitiveType)
	{
		return myModelFactory.CreatePrimitive(aPrimitiveType);
	}

	// Returns a newly created Camera
	_MSE_LOAD_CONTENT(SE::CCamera)
	{
		return myCameraFactory.CreateCamera();
	}

	// Create environtment light
	_MSE_LOAD_CONTENT(SE::CEnvironmentLight, const char* aPath)
	{
		return myLightFactory.CreateEnvironmentLight(aPath);
	}
	// Create point light
	_MSE_LOAD_CONTENT(SE::CPointLight)
	{
		return myLightFactory.CreatePointLight();
	}
	// Create spot light
	_MSE_LOAD_CONTENT(SE::CSpotLight)
	{
		return myLightFactory.CreateSpotLight();
	}

	// Load Texture
	_MSE_LOAD_CONTENT(SE::CTexture, const char* aPath)
	{
		return myTextureFactory.LoadTexture(aPath);
	}

#undef _MSE_CONTENT_LOAD
}
