#include "Input.h"
#include "Keyboard.h"
#include <Windows.h>
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
	BI[eButtonInput::Up] = IV({ VK_UP, 'W' });
	BI[eButtonInput::Down] = IV({ VK_DOWN, 'S' });
	BI[eButtonInput::Enter] = IV({ VK_RETURN, VK_SPACE });
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

void Input::Update()
{
	myKeyboard.Update();
}

void Input::AddKey(unsigned int aKey)
{
	myKeyboard.AddKey(aKey);
}
