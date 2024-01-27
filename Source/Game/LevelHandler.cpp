#include "pch.h"
#include "LevelHandler.h"
#include <fstream>
#include "JsonDocument.h"

#include <Engine/Engine.h>
#include <Engine/GraphicsEngine.h>
#include "Expose.h"

#include <ImGui/imgui.h>

#include <Engine/EnvironmentLight.h>
#include <Engine/Camera.h>
#include "UIManager.h"
#include "StateCutscene.h"
#include "StateStack.h"
#include <Engine\VFXManager.h>
#include <Engine\SpriteFactory.h>
#include <Engine\ContentLoader.h>

extern SE::CEnvironmentLight* _locLight;
extern SE::CCamera* _locCamera;

void LevelHandler::LoadLevel(int anIndex)
{
	SE::CVFXManager::GetInstance().ClearVFXs();
	if (myLevel)
		delete myLevel;
	myLevel = new Level();
	myLevel->Init(&myExperience, myLoadLevelFromLevelSelect, anIndex);
	myLevel->PopulateFromJson(myLevelPaths[anIndex]);

	//Set references
	auto& playerSys = myLevel->GetPlayerSystem();
	Singleton<UIManager>().SetNewPlayerReferences(&playerSys.GetHealthComponent()->GetHealth(), &playerSys.GetAPComponent()->GetAttackPower());

	//for cutscene
	LevelSettings& levelSettings = myLevelSettings[myActiveLevelIndex];

	_locCamera->SetRenderOffset(myLevel->GetPlayerSystem().GetCurrentPosition());
	_locCamera->SetOrbit(levelSettings.cameraRotation, levelSettings.cameraOffset, levelSettings.cameraDistance);
	//myLoadingScreen->Render();
	//if (anIndex == 4)
	//{
	//	myLevel->GetBossSystem().Init(myLevel->GetCoordinator(), &myLevel->GetPlayerSystem(), //AudioSystem::GetInstance());
	//}
}

LevelHandler::~LevelHandler()
{
	myLoadingScreen->Release();
}

void LevelHandler::Init(const std::string& aLevelConfig)
{
	myLoadingScreen = SE::CEngine::GetInstance()->GetContentLoader()->GetSpriteFactory().GetSprite("Textures/Sprites/UI/UI_S_loadingScreen.dds");
	//myLoadingScreen->SetPosition({ 0, 0 });
	//myLoadingScreen->SetSizeRelativeToScreen({1, 1});
	myExperience.Init();
	JsonDocument json(aLevelConfig);

	auto levels = json.GetDocument()["Levels"].GetArray();
	myLevelSettings.reserve(levels.Size());

	int levelIndex = 0;
	for (auto& jsonLevel : levels)
	{
		std::string levelPath = jsonLevel.GetString();
		myLevelPaths.push_back(levelPath);
		myLevelSettings.emplace_back();
	}
	LoadLevel(levelIndex);

	for (size_t i = 0; i < myLevelSettings.size(); i++)
	{
		std::string category = "Level " + std::to_string(i + 1);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].alphaThreshold,
			"Alpha Threshold%", "> Alpha Threshold%", category);

		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].waterLevel,
			"Water Level", "> Water Level", category);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].shallowWaterColor,
			"Shallow Color", "> Shallow Color", category, JsonExposedImguiTag::ColorPicker);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].deepWaterColor,
			"Deep Color", "> Deep Color", category, JsonExposedImguiTag::ColorPicker);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].waterBorderColor,
			"Border Color", "> Border Color", category, JsonExposedImguiTag::ColorPicker);

		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].waterBorderThreshold,
			"Border Threshold", "> Border Threshold", category);

		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].depthMult,
			"Depth Multiplier", "> Depth Multiplier", category);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].alphaMult,
			"Alpha Multiplier", "> Alpha Multiplier", category);

		// fog
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].fogColor,
			"Fog Color", "> Fog Color", category, JsonExposedImguiTag::ColorPicker);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].fogMultiplier,
			"Fog Multiplier", "> Fog Multiplier", category);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].fogDensity,
			"Fog Density", "> Fog Density", category);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].fogOffset,
			"Fog Offset", "> Fog Offset", category);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].godRaySampleDistance,
			"God Ray Sample Distance", "> God Ray Sample Distance", category);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].godRayFalloff,
			"God Ray Falloff", "> God Ray Falloff", category);

		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].lightColor.rgb,
			"Light Color", "> Light Color", category, JsonExposedImguiTag::ColorPicker);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].lightColor.a,
			"Light Intensity", "> Light Intensity", category);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].lightDirection.xy,
			"Light Direction", "> Light Direction", category);

		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].ambientColor.rgb,
			"Ambient Color", "> Ambient Color", category, JsonExposedImguiTag::ColorPicker);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].ambientColor.a,
			"Ambient Intensity", "> Ambient Intensity", category);

		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].cameraRotation,
			"Camera Rotation", "> Camera Rotation", category);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].cameraDistance,
			"Camera Distance", "> Camera Distance", category);
		Singleton<JsonManager>().ExposeVariableMacro(&myLevelSettings[i].cameraOffset,
			"Camera Offset", "> Camera Offset", category);
	}

	// WELCOME     ABOMINATION
	//         THE
	#define ABOMINATION_EXPOSE_LEVEL_SETTINGS(index, viewIndex)\
	if (myLevelSettings.size()>index) {\
		EXPOSE(myLevelSettings[index].alphaThreshold,        "LVL" #viewIndex " Alpha Threshold%");\
		EXPOSE(myLevelSettings[index].waterLevel,        "LVL" #viewIndex " Water Level");\
		EXPOSE(myLevelSettings[index].shallowWaterColor, "LVL" #viewIndex " Shallow Color", JsonExposedImguiTag::ColorPicker);\
		EXPOSE(myLevelSettings[index].deepWaterColor,    "LVL" #viewIndex " Deep Color", JsonExposedImguiTag::ColorPicker);\
		EXPOSE(myLevelSettings[index].waterBorderColor,  "LVL" #viewIndex " Border Color", JsonExposedImguiTag::ColorPicker);\
		EXPOSE(myLevelSettings[index].waterBorderThreshold,  "LVL" #viewIndex " Border Threshold");\
		EXPOSE(myLevelSettings[index].depthMult,         "LVL" #viewIndex " Depth Multiplier");\
		EXPOSE(myLevelSettings[index].alphaMult,         "LVL" #viewIndex " Alpha Multiplier");\
		EXPOSE(myLevelSettings[index].fogColor,          "LVL" #viewIndex " Fog Color");\
		EXPOSE(myLevelSettings[index].fogMultiplier,     "LVL" #viewIndex " Fog Multiplier");\
		EXPOSE(myLevelSettings[index].fogDensity,	     "LVL" #viewIndex " Fog Density");\
		EXPOSE(myLevelSettings[index].fogOffset,	     "LVL" #viewIndex " Fog Offset");\
		EXPOSE(myLevelSettings[index].godRaySampleDistance, "LVL" #viewIndex " God Ray Sample Distance");\
		EXPOSE(myLevelSettings[index].godRayFalloff,	 "LVL" #viewIndex " God Ray Falloff");\
		EXPOSE(myLevelSettings[index].lightColor.rgb,    "LVL" #viewIndex " Light Color");\
		EXPOSE(myLevelSettings[index].lightColor.a,      "LVL" #viewIndex " Light Intensity");\
		EXPOSE(myLevelSettings[index].lightDirection,    "LVL" #viewIndex " Light Direction");\
		EXPOSE(myLevelSettings[index].cameraRotation,    "LVL" #viewIndex " Camera Rotation");\
		EXPOSE(myLevelSettings[index].cameraDistance,    "LVL" #viewIndex " Camera Distance");\
		EXPOSE(myLevelSettings[index].cameraOffset,		 "LVL" #viewIndex " Camera Offset");\
		EXPOSE(myLevelSettings[index].ambientColor.rgb,	 "LVL" #viewIndex " Ambient Color");\
		EXPOSE(myLevelSettings[index].ambientColor.a,	 "LVL" #viewIndex " Ambient Intensity");\
	}
	ABOMINATION_EXPOSE_LEVEL_SETTINGS(0, 1);
	ABOMINATION_EXPOSE_LEVEL_SETTINGS(1, 2);
	ABOMINATION_EXPOSE_LEVEL_SETTINGS(2, 3);
	ABOMINATION_EXPOSE_LEVEL_SETTINGS(3, 4);
	ABOMINATION_EXPOSE_LEVEL_SETTINGS(4, 5);
}

void LevelHandler::UpdateActiveLevel()
{
	SE::CEngine::GetInstance()->GetActiveScene()->SetLoadingFrame(false);
	if (myLoadLevelNextFrame)
	{
		myLoadLevelNextFrame = false;
		LoadLevel(myActiveLevelIndex);

	}

	myLevel->Update();
	
	// Creme de la Creme

	LevelSettings& levelSettings = myLevelSettings[myActiveLevelIndex];
	auto& pdd = SE::CEngine::GetInstance()->GetGraphicsEngine()->GetPostProcessingData();
	pdd.waterLevel = levelSettings.waterLevel;
	pdd.shallowWaterColor = levelSettings.shallowWaterColor;
	pdd.deepWaterColor = levelSettings.deepWaterColor;
	pdd.waterBorderColor = levelSettings.waterBorderColor;
	pdd.depthMult = levelSettings.depthMult;
	pdd.alphaMult = levelSettings.alphaMult;
	pdd.borderThreshhold = levelSettings.waterBorderThreshold;
	pdd.fogColor = levelSettings.fogColor;
	pdd.fogMult = levelSettings.fogMultiplier;
	pdd.fogDensity = levelSettings.fogDensity;
	pdd.fogOffset = levelSettings.fogOffset;
	pdd.godRayFalloff = levelSettings.godRayFalloff;
	pdd.godRaySampleDistance = levelSettings.godRaySampleDistance;
	pdd.alphaThreshold = levelSettings.alphaThreshold < 100 ? levelSettings.alphaThreshold : 99.99f;

	_locLight->SetDirectionalColor(levelSettings.lightColor);
	_locLight->SetAmbientColor(levelSettings.ambientColor);
	_locLight->SetRotation(levelSettings.lightDirection);

	_locCamera->SetOrbit(levelSettings.cameraRotation, levelSettings.cameraOffset, levelSettings.cameraDistance);
}
