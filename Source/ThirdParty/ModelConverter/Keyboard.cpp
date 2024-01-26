#include "Keyboard.h"
#include <Windows.h>
#include <algorithm>

#define IS_KEY_DOWN(key) GetAsyncKeyState(key)

Keyboard::Keyboard()
{
}

bool Keyboard::GetKeyPressed(unsigned int aKey)
{
	if (!KeyExists(aKey))
		return false;

	return (myKeys[aKey].myState == Key::eState::Pressed);
}

bool Keyboard::GetKeyHeld(unsigned int aKey)
{
	if (!KeyExists(aKey))
		return false;

	return (myKeys[aKey].myState == Key::eState::Held);
}

bool Keyboard::GetKeyReleased(unsigned int aKey)
{
	if (!KeyExists(aKey))
		return false;

	return (myKeys[aKey].myState == Key::eState::Released);
}

bool Keyboard::GetKeyDown(unsigned int aKey)
{
	if (!KeyExists(aKey))
		return false;

	return GetKeyPressed(aKey) || GetKeyHeld(aKey);
}

void Keyboard::Update()
{
	for (auto& key : myKeys)
	{
		switch (key.second.myState)
		{
		case Key::eState::Neutral:
			if (IS_KEY_DOWN(key.first))
			{
				myKeys[key.first].myState = Key::eState::Pressed;
			}
			break;
		case Key::eState::Pressed:
			if (!(IS_KEY_DOWN(key.first)))
			{
				key.second.myState = Key::eState::Released;
			}
			else
			{
				myKeys[key.first].myState = Key::eState::Held;
			}
			break;
		case Key::eState::Held:
			if (!(IS_KEY_DOWN(key.first)))
			{
				key.second.myState = Key::eState::Released;
			}
			break;
		case Key::eState::Released: key.second.myState = Key::eState::Neutral; break;
		}
	}
}

bool Keyboard::KeyExists(unsigned int aKey)
{
	return myKeys.find(aKey) != myKeys.end();
}
