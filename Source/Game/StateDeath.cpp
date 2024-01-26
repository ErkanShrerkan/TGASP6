#include "pch.h"
#include "StateDeath.h"
#include "UIButton.h"
#include "UIManager.h"
#include <Engine/Input.h>
#include "StateStack.h"
StateDeath::StateDeath()
{
	myStateID = eStateID::Pause;
	UIElement* background = new UIElement("Textures/Sprites/UI/UI_S_DeathScreen.dds");
	UIButton* respawnButton = new UIButton("Textures/Sprites/UI/UI_S_Resume.dds", true, eMessage::eRespawn);
	UIButton* returnMainButton = new UIButton("Textures/Sprites/UI/UI_S_MainMenu.dds", false, eMessage::ePopStackToMainMenu);
	respawnButton->SetPosition({ 0.35f, 0.7f });
	returnMainButton->SetPosition({ 0.365f, 0.85f });
	background->SetPosition({ 0.5f, 0.5f });
	background->SetPivot({ 0.5f, 0.5f });
	respawnButton->SetSize({ 0.55f, 0.55f });
	returnMainButton->SetSize({ 0.5f, 0.5f });
	AddUIElement(background);
	AddUIElement(respawnButton);
	AddUIElement(returnMainButton);
}

StateDeath::~StateDeath()
{

}

bool StateDeath::Update()
{
	UpdateUIElements();
	return false;
}

void StateDeath::Render()
{
	for (auto& element : myUIElements)
	{
		element->Render();
	}
}