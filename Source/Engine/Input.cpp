#include "pch.h"
#include "Input.h"
#include "Keyboard.h"
#include <Windows.h>
#include "GraphicsEngine.h"
#include "Engine.h"
#include <algorithm>

Keyboard Input::myKeyboard;
std::unordered_map<eButtonInput, Input::InputValue> Input::myButtonInputs;

#define BI myButtonInputs

Input::Input()
{
}

void Input::Init()
{
	typedef InputValue IV;
	BI[eButtonInput::Up] = IV({ VK_SPACE });
	BI[eButtonInput::Down] = IV({ VK_LCONTROL });
	BI[eButtonInput::Forward] = IV({ 'W' });
	BI[eButtonInput::Back] = IV({ 'S' });
	BI[eButtonInput::Left] = IV({ 'A' });
	BI[eButtonInput::Right] = IV({ 'D' });
	BI[eButtonInput::RotateLeft] = IV({ VK_LBUTTON, 'Q'});
	BI[eButtonInput::RotateRight] = IV({ VK_RBUTTON, 'E'});
	BI[eButtonInput::ToggleAllPasses] = IV({ VK_F1 });
	BI[eButtonInput::TogglePositionPass] = IV({ VK_F2 });
	BI[eButtonInput::ToggleAlbedoPass] = IV({ VK_F3 });
	BI[eButtonInput::ToggleNormalPass] = IV({ VK_F4 });
	BI[eButtonInput::ToggleVertexNormalPass] = IV({ VK_F5 });
	BI[eButtonInput::ToggleMaterialPass] = IV({ VK_F6 });
	BI[eButtonInput::ToggleAOPass] = IV({ VK_F7 });
	BI[eButtonInput::ToggleDepthPass] = IV({ VK_F8 });
	BI[eButtonInput::LBUTTON] = IV({ VK_LBUTTON });
	BI[eButtonInput::Escape] = IV({ VK_ESCAPE });
	BI[eButtonInput::UltimateAttack] = IV({ 0x34, 'F'});
	BI[eButtonInput::Dammsugaren] = IV({ 0x33, 'D'});
	BI[eButtonInput::Jump] = IV({ 0x32, 'S'});
	BI[eButtonInput::AoeAttack] = IV({ 0x31, 'A'});
	BI[eButtonInput::Heal] = IV({ 'Q' });
	BI[eButtonInput::RMBAttack] = IV({ VK_RBUTTON });
	BI[eButtonInput::LMBAttack] = IV({ VK_LBUTTON });
	BI[eButtonInput::PlayerWalk] = IV({ VK_MBUTTON });
	BI[eButtonInput::SpawnBossKeyP] = IV({ 'P' });
	BI[eButtonInput::SpawnMinibossKeyL] = IV({ 'L' });
	BI[eButtonInput::MovementToggle] = IV({ VK_SHIFT });
	BI[eButtonInput::SpawnEffect] = IV({ 'H' });
	BI[eButtonInput::DebugLevelUp] = IV({ 'I' });
	BI[eButtonInput::ToggleOutline] = IV({ 'B' });
	BI[eButtonInput::ToggleHighlight] = IV({ 'N' });

}

bool Input::GetInputPressed(eButtonInput anInput)
{
	bool pc = false;

	for (auto& input : myButtonInputs[anInput].myKeyInput)
	{
		pc = myKeyboard.GetKeyPressed(input);
		if (pc)
		{
			break;
		}
	}
	return pc;
}

bool Input::GetInputHeld(eButtonInput anInput)
{
	bool pc = false;
	for (auto& input : myButtonInputs[anInput].myKeyInput)
	{
		pc = myKeyboard.GetKeyHeld(input);
		if (pc)
		{
			break;
		}
	}
	return pc;
}

bool Input::GetInputReleased(eButtonInput anInput)
{
	bool pc = false;
	for (auto& input : myButtonInputs[anInput].myKeyInput)
	{
		pc = myKeyboard.GetKeyReleased(input);
		if (pc)
		{
			break;
		}
	}
	return pc;
}

bool Input::GetInputDown(eButtonInput anInput)
{
	bool pc = false;
	for (auto& input : myButtonInputs[anInput].myKeyInput)
	{
		pc = myKeyboard.GetKeyDown(input);
		if (pc)
		{
			break;
		}
	}
	return pc;
}

Vector2f Input::GetMousePos()
{
	POINT p;
	GetCursorPos(&p);
	RECT rect = {};
	HWND handle = GetActiveWindow();
	GetWindowRect(handle, &rect);
	ScreenToClient(handle, &p);
	return Vector2f({ (float)p.x / (rect.right - rect.left), (float)p.y / (rect.bottom - rect.top) });
}

Vector2f Input::GetClampedMousePos()
{
	POINT p;
	GetCursorPos(&p);
	RECT rect = {};
	HWND handle = GetActiveWindow();
	GetWindowRect(handle, &rect);
	ScreenToClient(handle, &p);
	Vector2f screenPos = { (float)p.x / (rect.right - rect.left), (float)p.y / (rect.bottom - rect.top) };
	return Vector2f({ std::clamp(screenPos.x,0.0f,1.0f),std::clamp(screenPos.y,0.0f,1.0f) });
}

void Input::Update()
{
	myKeyboard.Update();
}

void Input::AddKey(unsigned int aKey)
{
	myKeyboard.AddKey(aKey);
}
