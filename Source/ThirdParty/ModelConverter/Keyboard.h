#pragma once
#include <unordered_map>

struct Key
{
	enum class eState
	{
		Neutral,
		Pressed,
		Held,
		Released
	};

	Key() : myState(eState::Neutral) {}

	eState myState;
};

class Keyboard
{
  public:
	Keyboard();
	bool GetKeyPressed(unsigned int aKey);
	bool GetKeyHeld(unsigned int aKey);
	bool GetKeyReleased(unsigned int aKey);
	bool GetKeyDown(unsigned int aKey);
	void Update();
	void AddKey(unsigned int aKey) { myKeys[aKey]; };

  private:
	bool KeyExists(unsigned int aKey);

	std::unordered_map<unsigned int, Key> myKeys;
};
