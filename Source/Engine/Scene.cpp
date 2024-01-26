#include "pch.h"
#include "Scene.h"
#include "ModelInstance.h"
#include "PointLight.h"
#include "SpotLight.h"

#include "Camera.h"
#include "Model.h"

namespace SE
{
	void CScene::AddInstance(void* aCollection, Matrix4x4f aTransform, Vector3f aScale)
	{
		ModelCollection collection = *reinterpret_cast<ModelCollection*>(aCollection);

		if (myModelAndTransformIndex >= myModelAndTransforms.size())
		{
			myModelAndTransforms.push_back({collection, aTransform, aScale});
		}
		else
		{
			myModelAndTransforms[myModelAndTransformIndex].myCollection = collection;
			myModelAndTransforms[myModelAndTransformIndex].myTransform = aTransform;
			myModelAndTransforms[myModelAndTransformIndex].myScale = aScale;
		}
		myModelAndTransformIndex++;
	}
	void CScene::AddInstance(CModelInstance* anInstance)
	{
		//myModelInstances.push_back(anInstance);
		if (myModelIndex >= myModelInstances.size())
		{
			myModelInstances.push_back(anInstance);
		}
		else
		{
			myModelInstances[myModelIndex] = anInstance;
		}
		myModelIndex++;
	}

	void CScene::AddInstance(CCamera* aCamera)
	{
		myCameras.push_back(aCamera);
	}

	void CScene::AddInstance(CEnvironmentLight* anEnvironmentLight)
	{
		myEnvironmentLight = anEnvironmentLight;
	}

	void CScene::AddInstance(CPointLight* aPointLight)
	{
		if (myPointLightIndex >= myPointLights.size())
		{
			myPointLights.push_back(aPointLight);
		}
		else
		{
			myPointLights[myPointLightIndex] = aPointLight;
		}
		myPointLightIndex++;
	}

	void CScene::AddInstance(CSpotLight* aSpotLight)
	{
		mySpotLights.push_back(aSpotLight);
	}

	void CScene::AddInstance(CSprite* aSprite)
	{
		//mySprites.push_back(aSprite);
		if (mySpriteIndex >= mySprites.size())
		{
			mySprites.push_back(aSprite);
		}
		else
		{
			mySprites[mySpriteIndex] = aSprite;
		}
		mySpriteIndex += !myLoadingFrame;
	}

	void CScene::AddInstance(std::vector<CSprite*> someSprite)
	{
		//mySprites.push_back(aSprite);
		for (auto& sprite : someSprite)
		{
			if (mySpriteIndex >= mySprites.size())
			{
				mySprites.push_back(sprite);
			}
			else
			{
				mySprites[mySpriteIndex] = sprite;
			}
			mySpriteIndex++;
		}
		
	}

	void CScene::AddInstance(CParticleEmitterInstance* aParticleEmitterInstance)
	{
		if (myParticleEmitterIndex >= myParticleEmitterInstances.size())
		{
			myParticleEmitterInstances.push_back(aParticleEmitterInstance);
		}
		else
		{
			myParticleEmitterInstances[myParticleEmitterIndex] = aParticleEmitterInstance;
		}
		myParticleEmitterIndex++;
	}

	void CScene::AddInstance(std::shared_ptr<Text> aText)
	{
		myTexts.push_back(aText);
	}

	void CScene::RemoveInstance(CModelInstance* anInstance)
	{
		for (std::vector<CModelInstance*>::iterator instance = myModelInstances.begin();
			instance != myModelInstances.end();
			++instance)
		{
			if (anInstance == *instance)
			{
				std::iter_swap(instance, myModelInstances.end() - 1);
				myModelInstances.pop_back();
				break;
			}
		}
	}

	void CScene::PrepareFrame()
	{
		mySpriteIndex = 0;
		myModelIndex = 0;
		myModelAndTransformIndex = 0;
		myPointLightIndex = 0;
		myParticleEmitterIndex = 0;
	}

	void CScene::SetLoadingFrame(bool aLoadingFrame)
	{
		myLoadingFrame = aLoadingFrame;
	}

	void CScene::SetMainCamera(CCamera* aCamera)
	{
		myMainCamera = aCamera;
	}

	CCamera *const& CScene::GetMainCamera()
	{
		return myMainCamera;
	}

	CEnvironmentLight* const& CScene::GetEnvironmentLight()
	{
		return myEnvironmentLight;
	}

	std::vector<CSprite*>& CScene::GetSprites()
	{
		mySpritesToRender = std::vector<CSprite*>(mySprites.begin(), mySprites.begin() + mySpriteIndex);

		if (myLoadingFrame)
		{
			int i = 0;
			i;
		}

		return mySpritesToRender;
	}

	std::vector<CModelInstance*>& CScene::CullModels()
	{
		// TODO: Cull models
		myModelsToRender = std::vector<CModelInstance*>(myModelInstances.begin(), myModelInstances.begin() + myModelIndex);
		return myModelsToRender;
	}

	std::vector<ModelCollectionExtended>& CScene::GetModels()
	{
		myModelAndTransformsToRender.clear();
		for (auto& collection : myModelAndTransforms)
		{
			if (!collection.myCollection.model)
				continue;

			if (myMainCamera->PassesCulling(collection.myTransform.GetPosition(), collection.myCollection.model->GetRadius()))
			{
				myModelAndTransformsToRender.push_back(collection);
			}
		}
		return myModelAndTransformsToRender;
	}
	std::vector<CParticleEmitterInstance*>& CScene::GetParticleEmitters()
	{
		myParticleEmitterInstancesToRender = std::vector<CParticleEmitterInstance*>(myParticleEmitterInstances.begin(), myParticleEmitterInstances.begin() + myParticleEmitterIndex);
		return myParticleEmitterInstancesToRender;
	}
	std::pair<size_t, std::array<CPointLight*, MAX_POINT_LIGHTS>> CScene::CullPointLights(CModelInstance* const& aModel)
	{
		(void)aModel;

		// TODO: Cull lights based on their range and the models size
		std::array<CPointLight*, MAX_POINT_LIGHTS> lights{};
		size_t max = myPointLights.size() > MAX_POINT_LIGHTS ? MAX_POINT_LIGHTS : myPointLights.size();
		for (size_t i = 0; i < max; i++)
		{
			lights[i] = myPointLights[i];
		}
		return { max, lights };
	}
	std::pair<size_t, std::array<CSpotLight*, MAX_SPOT_LIGHTS>> CScene::CullSpotLights(CModelInstance* const& aModel)
	{
		(void)aModel;

		// TODO: Cull lights based on their range and the models size
		std::array<CSpotLight*, MAX_SPOT_LIGHTS> lights{};
		size_t max = mySpotLights.size() > MAX_SPOT_LIGHTS ? MAX_SPOT_LIGHTS : mySpotLights.size();
		for (size_t i = 0; i < max; i++)
		{
			lights[i] = mySpotLights[i];
		}
		return { max, lights };
	}
	const std::vector<CPointLight*>& CScene::CullPointLights()
	{
		myPointLightsToRender = std::vector<CPointLight*>(myPointLights.begin(), myPointLights.begin() + myPointLightIndex);
		return myPointLightsToRender;
	}
	const std::vector<CSpotLight*>& CScene::CullSpotLights()
	{
		return mySpotLights;
	}
	std::vector<std::shared_ptr<Text>>&  CScene::GetTexts()
	{
		return myTexts;
	}
}
