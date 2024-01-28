#include <windows.h>
#include <Engine/Engine.h>
#include <Engine/Timer.h>
#include <cmath>
#include <string>
#include <Game/Game.h>
#include "Engine/Input.h"
#include <Game/UIManager.h>
#include <Game/UIElement.h>
#define USE_PIX 0
#include "pix3.h"
#include <Engine\DX11.h>
#include <Engine\GraphicsEngine.h>
#include <Engine\WindowHandler.h>
#include <Engine\SplashScreen.h>

using namespace CommonUtilities;

#include <fcntl.h>
void InitConsoleToFile()
{
	FILE* fDummy;
	freopen_s(&fDummy, "console_log.txt", "w", stdout);
}

void InitConsole()
{
	AllocConsole();
	FILE* fDummy;
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	SetConsoleTitle(L"Print Logger for Student Engine");
	printf("Use pout instead of printf as this window is used in case of an unexpected crasch rather than as an actual debugger.\n");
	printf("=======================\n");
}

void HandleTabbing(bool& isTabbed)
{
	HWND active = GetActiveWindow();
	HWND foreground = GetForegroundWindow();
	bool same = active == foreground/*!memcmp(&active, &foreground, sizeof(HWND))*/;
	if (!same)
	{
		isTabbed = true;
	}
	else
	{
		Input::Update();
	}

	if (isTabbed && same)
	{
		isTabbed = false;
	}
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	hInstance; hPrevInstance; lpCmdLine; nCmdShow;

#ifdef _DEBUG
	InitConsole();
#endif // _DEBUG

	SE::CEngine::SEngineParameters engineParameters{};
	engineParameters.window.x = 0;
	engineParameters.window.y = 0;
	unsigned int y = GetSystemMetrics(SM_CYSCREEN);
	engineParameters.window.width = static_cast<int>((16.f / 9) * y);
	engineParameters.window.height = static_cast<int>(y);
	engineParameters.window.title = L"Spite";
	engineParameters.clearColor = { 1.0f, .85f, .66f, 1.f };
	//engineParameters.clearColor = { 64.f / 255.f, 127.f / 255.f, 1.f, 1.f };
	engineParameters.clearColor = { .1f, .1f, .1f, 1.f };

	// Start the Engine
	if (!SE::CEngine::GetInstance()->Start(engineParameters))
	{
		/* Error, could not start the engine */
		return 0xdead;
	}
	SE::CEngine* engine = SE::CEngine::GetInstance();

	Game::Game game;
	if (!game.Init())
	{
		/* Error, could not start the game */
		return 0xbeef;
	}

	SE::DX11::Graphics->GetWindowHandler().InitSubscribtions();

	bool isRunning = true;
	bool isTabbed = false;
	MSG windowMessage = { 0 };
	Timer timer;
	timer.Update();

#ifdef _RELEASE
	#define SPLASH
#endif // _RELEASE

#ifdef SPLASH
	SplashScreen* ss = new SplashScreen();
	bool isSplashing = true;
	while (isSplashing)
	{
		isSplashing = ss->GetState() != SplashScreen::eState::Over;

		while (PeekMessage(&windowMessage, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&windowMessage);
			DispatchMessage(&windowMessage);

			if (windowMessage.message == WM_QUIT)
			{
				isSplashing = false;
				isRunning = false;
			}
		}

		timer.Update();
		float dt = timer.GetDeltaTime();

		// Main Loop
		engine->BeginFrame();
		ss->Update(dt);
		ss->Render();
		engine->Render();
		engine->EndFrame();
	}
	delete ss;
	ShowCursor(false);
#endif // RELEASE

	Input::Init();
	while (isRunning)
	{
		PIXBeginEvent(PIX_COLOR_INDEX(128), __FUNCTION__);
		while (PeekMessage(&windowMessage, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&windowMessage);
			DispatchMessage(&windowMessage);

			if (windowMessage.message == WM_QUIT)
			{
				isRunning = false;
			}
		}

		HandleTabbing(isTabbed);

		timer.Update();
		float dt = timer.GetDeltaTime();

		// Main Loop
		engine->BeginFrame();
		game.Update();
		engine->Update(dt);
		engine->Render();
		//game.Render();
		engine->EndFrame();
		PIXEndEvent();
	}

	return 0;
}
