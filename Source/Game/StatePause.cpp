#include "pch.h"
#include "StatePause.h"
#include "UIButton.h"
#include "UIManager.h"
#include <Engine/Input.h>
#include "StateStack.h"
#include "AudioSystem.h"
StatePause::StatePause()
{
	myStateID = eStateID::Pause;
	UIButton* resumeButton = new UIButton("Textures/Sprites/UI/UI_S_Resume.dds", true, eMessage::ePopStack);
	UIButton* settingButton = new UIButton("Textures/Sprites/UI/UI_S_Options.dds", false, eMessage::eOpenSettings);
	UIButton* returnMainButton = new UIButton("Textures/Sprites/UI/UI_S_MainMenu.dds", false, eMessage::ePopStackToMainMenu);
	resumeButton->SetPosition({ 0.35f, 0.25f });
	settingButton->SetPosition({ 0.365f, 0.5f });
	returnMainButton->SetPosition({ 0.365f, 0.75f });
	resumeButton->SetSize({ 0.55f, 0.55f });
	settingButton->SetSize({ 0.5f, 0.5f });
	returnMainButton->SetSize({ 0.5f, 0.5f });
	AddUIElement(resumeButton);
	AddUIElement(settingButton);
	AddUIElement(returnMainButton);
}

StatePause::~StatePause()
{
}

bool StatePause::Update()
{
	UpdateUIElements();
	if (Input::GetInstance().GetInputPressed(eButtonInput::Escape))
	{

		if (Singleton<UIManager>().GetStateStack().GetCurrentState().GetStateID() == eStateID::Pause)
		{
			AudioSystem::GetInstance()->SetSnapShot(AudioSnapShot::PauseGame, false);
		}
		Singleton<UIManager>().GetStateStack().Pop();
	}
    return false;
}

void StatePause::Render()
{
	for (auto& element : myUIElements)
	{
		element->Render();
	}
}