#pragma once
#include "ShaderDefines.h"
#include <array>
#include <Engine\Transform.h>
#include <memory>
// TODO: Replace vector with set
class Text;
namespace SE
{
	class CModelInstance;
	class CModel;
	class CAnimator;
	class CCamera;
	class CEnvironmentLight;
	class CPointLight;
	class CSpotLight;
	class CSprite;
	class CParticleEmitterInstance;

	enum EntityType
	{
		eStatic,
		eEnemy,
		ePlayer,
		eEntityCount,
	};

	enum ShaderType
	{
		eNone = 1,
		eOutline = 2,
		eHighlight = 4,
	
		eShaderCount = 8,
	};

	inline ShaderType operator|(ShaderType a, ShaderType b)
	{
		return static_cast<ShaderType>(static_cast<int>(a) | static_cast<int>(b));
	}

	struct ModelCollection
	{
		CModel* model;
		CAnimator* animator;

		EntityType entityType = eStatic;
		unsigned shaderType = 1;
		float4 color = { 1, 1, 1, 1 };
	};
	struct ModelCollectionExtended
	{
		ModelCollection myCollection;
		Matrix4x4f myTransform;
		Vector3f myScale;
	};

	class CScene
	{
	public:

		void AddInstance(void* aCollection, Matrix4x4f aTransform, Vector3f aScale = {1, 1, 1});
		void AddInstance(CModelInstance* anInstance);
		void AddInstance(CCamera* aCamera);
		void AddInstance(CEnvironmentLight* anEnvironmentLight);
		void AddInstance(CPointLight* aPointLight);
		void AddInstance(CSpotLight* aSpotLight);
		void AddInstance(CSprite* aSprite);
		void AddInstance(std::vector<CSprite*> someSprite);
		void AddInstance(CParticleEmitterInstance* aParticleEmitterInstance);
		void AddInstance(std::shared_ptr<Text> aText);
		void RemoveInstance(CModelInstance* anInstance);
		void PrepareFrame();
		void SetLoadingFrame(bool aLoadingFrame);

		void SetMainCamera(CCamera* aCamera);
		CCamera* const& GetMainCamera();
		CEnvironmentLight* const& GetEnvironmentLight();

		std::vector<CSprite*>& GetSprites();
		std::vector<CModelInstance*>& CullModels();
		std::vector<ModelCollectionExtended>& GetModels();
		std::vector<CParticleEmitterInstance*>& GetParticleEmitters();
		std::pair<size_t, std::array<CPointLight*, MAX_POINT_LIGHTS>> CullPointLights(CModelInstance* const& aModel);
		std::pair<size_t, std::array<CSpotLight*, MAX_SPOT_LIGHTS>> CullSpotLights(CModelInstance* const& aModel);
		const std::vector<CPointLight*>& CullPointLights();
		const std::vector<CSpotLight*>& CullSpotLights();
		std::vector<std::shared_ptr<Text>>& GetTexts();
	private:
		CCamera* myMainCamera = nullptr;
		CEnvironmentLight* myEnvironmentLight = nullptr;
		// Maybe use std::set instead of vectors
		std::vector<CCamera*> myCameras;
		std::vector<ModelCollectionExtended> myModelAndTransforms;
		std::vector<ModelCollectionExtended> myModelAndTransformsToRender;
		std::vector<CModelInstance*> myModelInstances;
		std::vector<CModelInstance*> myModelsToRender;
		std::vector<CPointLight*> myPointLights;
		std::vector<CPointLight*> myPointLightsToRender;
		std::vector<CSpotLight*> mySpotLights;
		std::vector<CSprite*> mySprites;
		std::vector<CSprite*> mySpritesToRender;
		std::vector<CParticleEmitterInstance*> myParticleEmitterInstances;
		std::vector<CParticleEmitterInstance*> myParticleEmitterInstancesToRender;
		std::vector<std::shared_ptr<Text>> myTexts;
		uint mySpriteIndex = 0;
		uint myModelIndex = 0;
		uint myModelAndTransformIndex = 0;
		uint myPointLightIndex = 0;
		uint myParticleEmitterIndex = 0;
		bool myLoadingFrame = false;
	};
}
