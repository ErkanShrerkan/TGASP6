#include "pch.h"
#include "StateLevelSelect.h"
#include "UIButton.h"
#include "UIManager.h"
#include <Engine\Input.h>
#include "StateStack.h"
StateLevelSelect::StateLevelSelect()
{
	myStateID = eStateID::LevelSelect;
	UIButton* levelOne = new UIButton("Textures/Sprites/UI/UI_S_Level1.dds", false, eMessage::eStartLevel1, true);
	UIButton* levelTwo = new UIButton("Textures/Sprites/UI/UI_S_Level2.dds", false, eMessage::eStartLevel2, true);
	UIButton* levelThree = new UIButton("Textures/Sprites/UI/UI_S_Level3.dds", false, eMessage::eStartLevel3, true);
	UIButton* levelFour = new UIButton("Textures/Sprites/UI/UI_S_Level4.dds", false, eMessage::eStartLevel4, true);
	UIButton* levelFive = new UIButton("Textures/Sprites/UI/UI_S_Level5.dds", false, eMessage::eStartLevel5, true);
	UIButton* back = new UIButton("Textures/Sprites/UI/UI_S_Return.dds", true, eMessage::ePopStack);
	UIElement* background = new UIElement("Textures/Sprites/UI/UI_S_MenuBackground.dds");

	levelOne->SetPosition({ 0.325f, 0.15f });
	levelTwo->SetPosition({ 0.325f, 0.27f });
	levelThree->SetPosition({ 0.325f, 0.39f });
	levelFour->SetPosition({ 0.325f, 0.51f });
	levelFive->SetPosition({ 0.325f, 0.63f });
	levelOne->SetSize({ 0.65f, 0.5f });
	levelTwo->SetSize({ 0.65f, 0.5f });
	levelThree->SetSize({ 0.65f, 0.5f });
	levelFour->SetSize({ 0.65f, 0.5f });
	levelFive->SetSize({ 0.65f, 0.5f });

	back->SetPosition({ 0.35f, 0.8f });
	back->SetSize({ 0.55f, 0.55f });

	background->SetPosition({ 0.5f, 0.5f });
	background->SetPivot({ 0.5f, 0.5f });
	AddUIElement(background);
	AddUIElement(levelOne);
	AddUIElement(levelTwo);
	AddUIElement(levelThree);
	AddUIElement(levelFour);
	AddUIElement(levelFive);
	AddUIElement(back);
}

StateLevelSelect::~StateLevelSelect()
{
	for (auto& element : myUIElements)
	{
		delete element;
	}
	myUIElements.clear();
}

bool StateLevelSelect::Update()
{
	UpdateUIElements();
	if (Input::GetInstance().GetInputPressed(eButtonInput::Escape))
	{
		Singleton<UIManager>().GetStateStack().Pop();
	}
	return true;
}

void StateLevelSelect::Render()
{
	for (auto& element : myUIElements)
	{
		element->Render();
	}
}
