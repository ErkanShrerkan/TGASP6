#pragma once
#include <vector>
#include <unordered_map>
#include "pch.h"

class Keyboard;

enum class eButtonInput
{
	Up,
	Down,
	Forward,
	Back,
	Left,
	Right,
	RotateRight,
	RotateLeft,
	LBUTTON,
	Escape,
	Jump,
	Heal,
	AoeAttack,
	LMBAttack,
	RMBAttack,
	Dammsugaren,
	UltimateAttack,
	StandardAttack,
	ToggleAllPasses,
	TogglePositionPass,
	ToggleAlbedoPass,
	ToggleNormalPass,
	ToggleVertexNormalPass,
	ToggleMaterialPass,
	ToggleAOPass,
	ToggleDepthPass,
	PlayerWalk,
	SpawnBossKeyP,
	SpawnMinibossKeyL,
	MovementToggle,
	SpawnEffect,
	DebugLevelUp,
	ToggleOutline,
	ToggleHighlight,
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
	static float2 GetMousePos();
	static float2 GetClampedMousePos();

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