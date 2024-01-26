#include "pch.h"
#include "StateMainMenu.h"
#include "UIButton.h"
#include "UIManager.h"
#include <Engine\Input.h>
#include <memory>
#include <Engine/Engine.h>
#include <Engine/Scene.h>
#include <Engine/Camera.h>
#include "GameWorld.h"
StateMainMenu::StateMainMenu()
{
	myStateID = eStateID::MainMenu;
	UIButton* playButton = new UIButton("Textures/Sprites/UI/UI_S_EnterGame.dds", true, eMessage::eStartGame);
	UIButton* settingButton = new UIButton("Textures/Sprites/UI/UI_S_Options.dds", false, eMessage::eOpenSettings);
	UIButton* exitButton = new UIButton("Textures/Sprites/UI/UI_S_ExitGame.dds", false, eMessage::eQuitGame);
	UIButton* levelSelect = new UIButton("Textures/Sprites/UI/UI_S_LevelSelect.dds", false, eMessage::eOpenLevelSelect);
	UIButton* creditsButton = new UIButton("Textures/Sprites/UI/UI_S_Credits.dds", false, eMessage::eOpenCredits);
	UIElement* icon = new UIElement("Textures/Sprites/UI/UI_S_GameLogo.dds");
	UIElement* name = new UIElement("Textures/Sprites/UI/UI_S_TitleText.dds");
	UIElement* background = new UIElement("Textures/Sprites/UI/UI_S_MenuBackground.dds");
	playButton->SetPosition({ 0.085f, 0.4f });
	levelSelect->SetPosition({ 0.1f, 0.53f });
	settingButton->SetPosition({ 0.1f, 0.65f });
	exitButton->SetPosition({ 0.1f, 0.77f });
	creditsButton->SetPosition({ 0.7f, 0.8f });
	icon->SetPosition({ 0.1f, -0.02f });
	name->SetPosition({ -0.03f, 0.1f });
	background->SetPosition({ 0.5f, 0.5f });
	background->SetPivot({ 0.5f, 0.5f });
	playButton->SetSize({ 0.55f, 0.55f });
	levelSelect->SetSize({ 0.5f, 0.5f });
	settingButton->SetSize({ 0.5f, 0.5f });
	exitButton->SetSize({ 0.5f, 0.5f });
	creditsButton->SetSize({ 0.5f, 0.5f });
	//AddUIElement(background);
	AddUIElement(playButton);
	AddUIElement(levelSelect);
	AddUIElement(settingButton);
	AddUIElement(exitButton);
	AddUIElement(creditsButton);
	AddUIElement(icon);
	AddUIElement(name);
}

StateMainMenu::~StateMainMenu()
{
	for (auto& element : myUIElements)
	{
		delete element;
	}
	myUIElements.clear();
}

bool StateMainMenu::Update()
{
	int index = GameWorld::GetInstance()->GetLevelHandler().GetActiveLevelIndex();
	CAMERA->SetRenderOffset(myCameraPositions[index]);
	switch (index)
	{
	case 0:
		CAMERA->SetOrbit({ 9.f, 90.f }, { -629.f, -72.f }, 4594.f);
		break;
	case 1:
		CAMERA->SetOrbit({ 2.f, 350.f }, { -1587.f, 174.f }, -3063.f);
		break;
	case 2:
		CAMERA->SetOrbit({ -13.f, 135.f }, { -3719.f, -417.f }, -1907.f);
		break;
	case 3:
		CAMERA->SetOrbit({ 22.f, 306.f }, { -1938.f, -3254.f }, 12290.f);
		break;
	case 4:
		CAMERA->SetRenderOffset(myCameraPositions[0]);
		CAMERA->SetOrbit({ 9.f, 90.f }, { -629.f, -72.f }, 4594.f);
		break;
	default:
		break;
	}

	UpdateUIElements();
	return true;
}

void StateMainMenu::Render()
{
	for (auto& element : myUIElements)
	{
		element->Render();
	}
}

void StateMainMenu::SetCamPos(float3 aPos, int aIndex)
{
	myCameraPositions[aIndex] = aPos;
}

float3 StateMainMenu::GetPosByIndex(int aIndex)
{
	return myCameraPositions[aIndex];
}
