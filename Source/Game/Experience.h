#pragma once
#include <array>

	class Experience
	{
	public:
		void Init();

		void SetLevel(const int& aLevel);

		void AddXP(const int& anEnemyType);
		void UpdateExposedValues();
		void CalculateEnemyXPWorth(const int& anEnemyType, float& anXPAmount);
		const float GetXP() { return myXP; }
		const int GetLevel() { return myLevel; }
		const float GetXPBarPosition();

		const float GetLevelWhatLevelUnlocksAbility(const int& anAbilityIndex);

		const bool& GetUltimateIsUnlocked() { return myUltimateIsUnlocked; }
		void SetUltimateIsUnlocked(const bool& aBool);

	private:
		float myXP;

		int myLevel = 1;

		std::array<float, 11> myXPForLevelUp;

		float xpForLevelUp2;
		float xpForLevelUp3;
		float xpForLevelUp4;
		float xpForLevelUp5;
		float xpForLevelUp6;
		float xpForLevelUp7;
		float xpForLevelUp8;
		float xpForLevelUp9;

		int myNextLevelIndex = 2;


		float kubbLeffeXP;
		float bogScytheXP;
		float cultistXP;
		float championXP;


		float levelToUnlockAOE;
		float levelToUnlockTeleport;
		float levelToUnlockHook;
		//float levelToUnlockUltimate;

		bool myUltimateIsUnlocked = false;
	};



