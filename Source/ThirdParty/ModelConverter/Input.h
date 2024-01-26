#pragma once
#include <vector>
#include <unordered_map>

class Keyboard;

enum class eButtonInput
{
	Up,
	Down,
	Enter,
};

class Input
{
	friend class InputValue;
public:
	static Input& GetInstance()
	{
		static Input instance;
		return instance;
	}

	static bool GetInputPressed(eButtonInput anInput);
	static bool GetInputHeld(eButtonInput anInput);
	static bool GetInputReleased(eButtonInput anInput);
	static bool GetInputDown(eButtonInput anInput);

	static void Init();
	static void Update();

	Input(Input const&) = delete;
	void operator=(Input const&) = delete;

private:
	Input();
	static void AddKey(unsigned int aKey);

	static Keyboard myKeyboard;
	struct InputValue
	{
		std::vector<unsigned int> myKeyInput;

		InputValue(std::vector<unsigned int> aKeyInput)
			: myKeyInput(aKeyInput)
		{
			for (auto key : aKeyInput)
			{
				AddKey(key);
			}
		}

		InputValue() : myKeyInput({ 0 }) {}

		bool operator<(const InputValue& aRHS) const noexcept
		{
			return this->myKeyInput[0] < aRHS.myKeyInput[0];
		}
	};
	static std::unordered_map<eButtonInput, InputValue> myButtonInputs;
};