#pragma once
//#include <Windows.h>
//#include <functional>

#include <CommonUtilities\Math\Vector4.hpp>

/*

	Use this as MainSingleton with sub "singletons"

*/

#include "WindowHandler.h" // Struct

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ImGuiContext;

namespace SE
{
	//using Callback			= std::function<void()>;
	//using Callback_WndProc	= std::function<LRESULT(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)>;

	class CContentLoader;
	class CGraphicsEngine;
	class CAudioEngine;
	class CScene;
	class CEditor;
	class CEngine
	{
	public:
		struct SEngineParameters
		{
			CWindowHandler::SWindowData window;
			CommonUtilities::Vector4<float> clearColor;
		};

		constexpr static CEngine*& GetInstance() noexcept { return myInstance; }
	private:
		static CEngine* myInstance;
		float myDeltaTime;

	public:
		bool Start(const SEngineParameters& someParameters);
		CEngine(const SEngineParameters& someParameters);
		~CEngine();

		bool Update(float aDeltaTime);
		void Render();
		void BeginFrame();
		void EndFrame();

		void SetActiveScene(CScene* const& aScene);
		CScene* const& GetActiveScene();
		ID3D11Device* const& GetDXDevice() const;
		ID3D11DeviceContext* const& GetDXDeviceContext() const;
		CGraphicsEngine* const& GetGraphicsEngine();
		CContentLoader* const& GetContentLoader();
		float GetDeltaTime() { return myDeltaTime; };

	private:
		bool InternalStart();

		SEngineParameters myEngineParameters;

		CContentLoader* myContentLoader;
		CGraphicsEngine* myGraphicsEngine;
		CAudioEngine* myAudioEngine;
		CScene* myScene;
		CEditor* myEditor;

	};
}
