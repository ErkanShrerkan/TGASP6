#include "pch.h"
#include "Game.h"
/*///
#include <Engine/CommonUtilities.h>
#include <Engine/DebugConsole.h>
////*///
#include <ImGui/imgui.h>
#include <memory>

// Postprocessing imgui
#include <Engine/GraphicsEngine.h>

#include <Engine/Engine.h>
#include <Engine/ContentLoader.h>
#include <Engine/Scene.h>
// vvv Content Loading
//#include "Model.h"
#include <Engine/ModelInstance.h>
#include <Engine/Camera.h>
#include <Engine/EnvironmentLight.h>
#include <Engine/PointLight.h>
#include <Engine/SpotLight.h>

//#include <Engine/AudioEngine.h>
//#include "AudioComponent.h"

#include "Engine/Input.h"
#include "UIManager.h"
#include "StateStack.h"
#include "State.h"
#include "Engine\Sprite.h"
#include "Engine\AudioEngine.h"
#include "Engine\Animator.h"

#include "Engine\Model.h"
#include "TextFactory.h"
#include "Text.h"
// Temp

SE::CCamera* _locCamera = nullptr;

SE::CEnvironmentLight* _locLight;


#include "Model.h"

#include "GameWorld.h"
#include "Engine\DX11.h"
#include "Engine\NavMeshLoader.h"

#include "AudioSystem.h"
#include "PlayerSystem.h"

#include <pix3.h>

#include "Expose.h"
#include "Engine\VFXManager.h"
namespace Game
{
	Game::~Game()
	{
		delete myGameWorld;
		myGameWorld = nullptr;
	}
	bool Game::Init()
	{

		Singleton<JsonManager>().InitDocument("Data/Config.json");

		// camera
		SE::CEngine* const& engine = SE::CEngine::GetInstance();
		SE::CContentLoader* const& content = engine->GetContentLoader();
		_locCamera = content->Load<SE::CCamera>();
		_locCamera->Init(60.f, { 1600.f, 900.f });
		//Light to make game not crash, maybe not be dependent on light?
		_locLight = content->Load<SE::CEnvironmentLight>("Textures/cube_1024_preblurred_angle3_Skansen3.dds");

		InitMainmenuScene();
		InitGameScene();

		Singleton<UIManager>().Init();
		Singleton<UIManager>().GetStateStack().PushState(eStateID::MainMenu);

		myGameWorld = new GameWorld();
		myGameWorld->Init(_locCamera);

		return true;
	}

	bool Game::Update()
	{
		PIXBeginEvent(PIX_COLOR_INDEX(0), __FUNCTION__);
		myGameWorld->Update();

		float dt = SE::CEngine::GetInstance()->GetDeltaTime();
		SE::CVFXManager::GetInstance().Update(dt);

		DisplayImGui();
		Singleton<UIManager>().Update();
		Singleton<UIManager>().Render();





		PIXEndEvent();
		return true;
	}

	void Game::DisplayImGui()
	{
#ifdef DEBUG
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("F1 Menu"))
			{
				Singleton<JsonManager>().OnImGui();

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
#endif // DEBUG
	}

	void Game::InitGameScene()
	{

		SE::CEngine* const& engine = SE::CEngine::GetInstance();
		SE::CScene* const& scene = engine->GetActiveScene();

		// scene
		scene->AddInstance(_locCamera);
		scene->SetMainCamera(_locCamera);
	}
	void Game::InitMainmenuScene()
	{
		SE::CEngine* const& engine = SE::CEngine::GetInstance();
		SE::CScene* const& scene = engine->GetActiveScene();

		scene->AddInstance(_locLight);
		scene->AddInstance(_locCamera);
		scene->SetMainCamera(_locCamera);

	}
}


