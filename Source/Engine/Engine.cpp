#include "pch.h"
#include "Engine.h"
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#define USE_PIX 1
#include "pix3.h"

#include "GraphicsEngine.h"
#include "ContentLoader.h"
#include "Scene.h"

#include "DebugProfiler.h"
#include "Editor.h"

// Shortcuts
#include "DX11.h"

// Experimental
#include "GameObject.h"
#include "Component.h"

#include "AudioEngine.h"

#include "Camera.h"

namespace SE
{
	CEngine* CEngine::myInstance = nullptr;
	
	bool CEngine::Start(const SEngineParameters& someParameters)
	{
		if (!myInstance)
		{
			myInstance = new CEngine(someParameters);
			return myInstance->InternalStart();
		}
		else
		{
			/* Error Message: Can't have two engines running at the same time */
		}
		
		//myAudioEngine = AudioEngine::GetInstance();
		return false;
	}

	CEngine::CEngine(const SEngineParameters& someParameters)
		: myEngineParameters(someParameters)
		, myGraphicsEngine(nullptr)
	{
		myContentLoader = new CContentLoader();
		DX11::Content = myContentLoader;
		myScene = new CScene();
		myEditor = new CEditor();
		myAudioEngine = CAudioEngine::GetInstance();
	}

	CEngine::~CEngine()
	{
		// Release resources
		delete myContentLoader;
		myContentLoader = nullptr;

		delete myGraphicsEngine;
		myGraphicsEngine = nullptr;

		delete myScene;
		myScene = nullptr;

		delete myEditor;
		myEditor = nullptr;
	}

	bool CEngine::Update(float aDeltaTime)
	{
		PIXBeginEvent(PIX_COLOR_INDEX(0), __FUNCTION__);
		myDeltaTime = aDeltaTime;
		//myAudioEngine->Update(myScene->GetMainCamera()->GetPosition());
		
		/*if (ImGui::Begin("Scene"))
		{
			int index = 0;
			for (auto& object : g_objects)
			{
				std::string nick(object.myNick);
				nick.append("##").append(std::to_string(index));

				if (ImGui::TreeNodeEx(nick.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen))
					{
						for (auto& pair : object.myComponents)
						{
							for (auto& component : pair.second)
							{
								component->OnGui();
								ImGui::Separator();
							}
						}
					}

					ImGui::TreePop();
				}
			}
		}
		ImGui::End();*/

		PIXEndEvent();
		return true;
	}

	void CEngine::Render()
	{
		PIXBeginEvent(PIX_COLOR_INDEX(0), __FUNCTION__);

		//*///
		myGraphicsEngine->RenderFrame();
		//*///

		/*///
		//PIXBeginEvent(PIX_COLOR_INDEX(1), "Debug::CDebugConsole::Render");
		pout.Render();
		//PIXEndEvent();
		//*///

		//*///
		PIXBeginEvent(PIX_COLOR_INDEX(1), "Debug::CDebugProfiler::Render");
		Singleton<Debug::CDebugProfiler>().Render();
		PIXEndEvent();
		//*///

		/*///
		//PIXBeginEvent(PIX_COLOR_INDEX(1), "CEditor::Render");
		myEditor->Render();
		//PIXEndEvent();
		//*///

		PIXEndEvent();
	}

	void CEngine::BeginFrame()
	{
		PIXBeginEvent(PIX_COLOR_INDEX(0), __FUNCTION__);
		GetActiveScene()->PrepareFrame();
		Singleton<Debug::CDebugProfiler>().BeginCapture();
		myGraphicsEngine->BeginFrame();
		PIXEndEvent();
	}

	void CEngine::EndFrame()
	{
		PIXBeginEvent(PIX_COLOR_INDEX(0), __FUNCTION__);
		myGraphicsEngine->EndFrame();
		Singleton<Debug::CDebugProfiler>().EndCapture();
		PIXEndEvent();
	}

	void CEngine::SetActiveScene(CScene* const& aScene)
	{
		myScene = aScene;
	}

	CScene* const& CEngine::GetActiveScene()
	{
		return myScene;
	}

	ID3D11Device* const& CEngine::GetDXDevice() const
	{
		return myGraphicsEngine->GetFramework().GetDevice();
	}

	ID3D11DeviceContext* const& CEngine::GetDXDeviceContext() const
	{
		return myGraphicsEngine->GetFramework().GetContext();
	}

	CGraphicsEngine* const& CEngine::GetGraphicsEngine()
	{
		return myGraphicsEngine;
	}

	CContentLoader* const& CEngine::GetContentLoader()
	{
		return myContentLoader;
	}

	bool CEngine::InternalStart()
	{
		/*g_objects.emplace_back();
		g_objects.emplace_back();
		GameObject& obj = g_objects.back();
		obj.myNick = "Marie";

		if (Static<SE::GameObjectInstancer>().SerializeGameObjectsToFile(g_objects, "experimental_objects.json"))
		{
			std::vector<GameObject> newObjects;
			Static<SE::GameObjectInstancer>().DeserializeGameObjectsFromFile(newObjects, "experimental_objects.json");

			return false;
		}*/

		myGraphicsEngine = new CGraphicsEngine();
		DX11::Graphics = myGraphicsEngine;
		bool shouldRun = myGraphicsEngine->Init(myEngineParameters.window);
		myGraphicsEngine->SetClearColor(myEngineParameters.clearColor);
		if (!shouldRun)
		{
			/* Error Message */
			return false;
		}

		return true;
	}
}
