#include "pch.h"
#include "Experience.h"
#include "Engine/AudioEngine.h"
#include "Postmaster.h"
#include "Expose.h"
#include <string>
void Experience::Init()
{
	EXPOSE(xpForLevelUp2);
	EXPOSE(xpForLevelUp3);
	EXPOSE(xpForLevelUp4);
	EXPOSE(xpForLevelUp5);
	EXPOSE(xpForLevelUp6);
	EXPOSE(xpForLevelUp7);
	EXPOSE(xpForLevelUp8);
	EXPOSE(xpForLevelUp9);

	UpdateExposedValues();
	myXPForLevelUp[10] = 1;

	EXPOSE(kubbLeffeXP);
	EXPOSE(bogScytheXP);
	EXPOSE(cultistXP);
	EXPOSE(championXP);


	EXPOSE(levelToUnlockAOE);
	EXPOSE(levelToUnlockTeleport);
	EXPOSE(levelToUnlockHook);
	//EXPOSE(levelToUnlockUltimate);

}
void Experience::SetLevel(const int& aLevel)
{
	myLevel = aLevel;
	myNextLevelIndex = 10;
	myXP = 1.0f;
}

void Experience::AddXP(const int& anEnemyType)
{
	UpdateExposedValues();

	if (myNextLevelIndex < 10)
	{
		float anXPAmount = 0;
		CalculateEnemyXPWorth(anEnemyType, anXPAmount);

		myXP += anXPAmount;
		float xpOver = 0;
		float xpRatio = myXP / myXPForLevelUp[myNextLevelIndex];
		SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_GainXP, { 0,0,0 }, false, "XP", xpRatio);
		if (myXP >= myXPForLevelUp[myNextLevelIndex])
		{
			xpOver = myXP - myXPForLevelUp[myNextLevelIndex];
			if (myNextLevelIndex == 9)
			{
				xpOver = myXPForLevelUp[myNextLevelIndex +1];
			}
			myXP = xpOver;
			myLevel++;
			myNextLevelIndex++;
			Postmaster::GetInstance()->SendMail(eMessage::eLevelUp);

		}
	}
	

}


const float Experience::GetXPBarPosition()
{
	if (myNextLevelIndex <= 10)
	{
		return myXP / myXPForLevelUp[myNextLevelIndex];
	}
	else
	{
		return 1.0f;
	}
}

const float Experience::GetLevelWhatLevelUnlocksAbility(const int& anAbilityIndex)
{
	switch (anAbilityIndex)
	{
	case 0:
		return levelToUnlockAOE;
		break;
	case 1:
		return levelToUnlockTeleport;
		break;
	case 2:
		return levelToUnlockHook;
		break;
	/*case 3:
		return levelToUnlockUltimate;
		break;*/
	default:
		return 0;
		break;
	}
}

void Experience::SetUltimateIsUnlocked(const bool& aBool)
{
	myUltimateIsUnlocked = aBool;
}

void Experience::CalculateEnemyXPWorth(const int& anEnemyType, float& anXPAmount)
{
	switch (anEnemyType)
	{
	case 0:
		anXPAmount = kubbLeffeXP;
		break;
	case 1:
		anXPAmount = bogScytheXP;
		break;
	case 2:
		anXPAmount = cultistXP;
		break;
	case 3:
		anXPAmount = championXP;
		break;
	default:
		break;
	}
}
void Experience::UpdateExposedValues()
{
	myXPForLevelUp[2] = xpForLevelUp2;
	myXPForLevelUp[3] = xpForLevelUp3;
	myXPForLevelUp[4] = xpForLevelUp4;
	myXPForLevelUp[5] = xpForLevelUp5;
	myXPForLevelUp[6] = xpForLevelUp6;
	myXPForLevelUp[7] = xpForLevelUp7;
	myXPForLevelUp[8] = xpForLevelUp8;
	myXPForLevelUp[9] = xpForLevelUp9;
}