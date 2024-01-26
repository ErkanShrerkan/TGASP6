#pragma once
#include "Stopwatch.h"

namespace SE
{
	class CSprite;
}

class SplashScreen
{
public:
	enum class eState
	{
		TGA,
		Group,
		FMOD,
		Over
	};

	SplashScreen();
	~SplashScreen();

	void Update(float aDeltaTime);
	void Render();

	eState GetState();

private:
	void UpdateTGA(float aDeltaTime);
	void UpdateGroup(float aDeltaTime);
	void UpdateFMOD(float aDeltaTime);

	float myTGAScale = 1.0f;
	float myGroupScale = 2.0f;
	float myFMODScale = 0.5;
	float myTGAAlpha = 1;
	float myGroupAlpha = 0;
	float myFMODAlpha = 0;
	SE::CSprite* myTGALogo;
	SE::CSprite* myGroupLogo;
	SE::CSprite* myFMODLogo;
	eState myState;
	Stopwatch myTGATimer;
	Stopwatch myGroupTimer;
	Stopwatch myFMODTimer;
};