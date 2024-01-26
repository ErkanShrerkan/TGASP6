#include "pch.h"
#include "StateSettings.h"
#include <Engine/Engine.h>
#include <Engine/Scene.h>

#include "UIButton.h"
#include "UISlider.h"
#include "UIManager.h"
#include <memory>
#include "StateStack.h"
#include <Engine/Sprite.h>
#include <Engine/Input.h>
StateSettings::StateSettings()
{
	myStateID = eStateID::Settings;
	UIButton* backButton = new UIButton("Textures/Sprites/UI/UI_S_Return.dds", true, eMessage::ePopStack);
	UISlider* masterVolumeSlider = new UISlider("Textures/Sprites/UI/UI_S_SliderBackground.dds", eMessage::eChangeMasterVolume);
	UISlider* musicVolumeSlider = new UISlider("Textures/Sprites/UI/UI_S_SliderBackground.dds", eMessage::eChangeMusicVolume);
	UISlider* SFXVolumeSlider = new UISlider("Textures/Sprites/UI/UI_S_SliderBackground.dds", eMessage::eChangeSFXVolume);
	UIElement* background = new UIElement("Textures/Sprites/UI/UI_S_MenuBackground.dds");
	UIElement* master = new UIElement("Textures/Sprites/UI/UI_S_MasterVolume.dds");
	UIElement* music = new UIElement("Textures/Sprites/UI/UI_S_Music.dds");
	UIElement* sfx = new UIElement("Textures/Sprites/UI/UI_S_SFX.dds");
	UIButton* res960  = new UIButton("Textures/Sprites/UI/UI_S_540.dds", false, eMessage::e960x540);
	UIButton* res1280 = new UIButton("Textures/Sprites/UI/UI_S_720.dds", false, eMessage::e1280x720);
	UIButton* res1600 = new UIButton("Textures/Sprites/UI/UI_S_900.dds", false, eMessage::e1600x900);
	UIButton* res1920 = new UIButton("Textures/Sprites/UI/UI_S_1080.dds", false, eMessage::e1920x1080);
	UIButton* res2560 = new UIButton("Textures/Sprites/UI/UI_S_1440.dds", false, eMessage::e2560x1440);

	res960->SetSize({ 0.25f, 0.25f });
	res1280->SetSize({ 0.25f, 0.25f });
	res1600->SetSize({ 0.25f, 0.25f });
	res1920->SetSize({ 0.25f, 0.25f });
	res2560->SetSize({ 0.25f, 0.25f });

	res960 ->SetPosition({ 0.75f, 0.3f});
	res1280->SetPosition({ 0.75f, 0.4f});
	res1600->SetPosition({ 0.75f, 0.5f});
	res1920->SetPosition({ 0.75f, 0.6f});
	res2560->SetPosition({ 0.75f, 0.7f});

	res960 ->GetSprite()->SetPosition({ 0.75f, 0.3f });
	res1280->GetSprite()->SetPosition({ 0.75f, 0.4f });
	res1600->GetSprite()->SetPosition({ 0.75f, 0.5f });
	res1920->GetSprite()->SetPosition({ 0.75f, 0.6f });
	res2560->GetSprite()->SetPosition({ 0.75f, 0.7f });

	

	background->SetPosition({ 0.5f, 0.5f });
	background->SetPivot({ 0.5f, 0.5f });

	backButton->SetPosition({ 0.365f, 0.8f });
	backButton->SetSize({ 0.55f, 0.55f });
	
	master->SetPosition({ 0.4f, 0.27f });
	master->SetPivot({ 0.5f, 0.5f });
	music->SetPosition({ 0.385f, 0.42f });
	music->SetPivot({ 0.5f, 0.5f });
	sfx->SetPosition({ 0.4f, 0.57f });
	sfx->SetPivot({ 0.5f, 0.5f });

	masterVolumeSlider->SetPosition({ 0.4f, 0.35f });
	masterVolumeSlider->SetPivot({ 0.f, 0.5f });
	masterVolumeSlider->InitWithValue(0.5f);
	musicVolumeSlider->SetPosition({ 0.4f, 0.5f });
	musicVolumeSlider->SetPivot({ 0.f, 0.5f });
	musicVolumeSlider->InitWithValue(0.5f);
	SFXVolumeSlider->SetPosition({ 0.4f, 0.65f });
	SFXVolumeSlider->SetPivot({ 0.f, 0.5f });
	SFXVolumeSlider->InitWithValue(0.5f);
	AddUIElement(background);
	AddUIElement(backButton);
	AddUIElement(master);
	AddUIElement(music);
	AddUIElement(sfx);
	AddUISlider(masterVolumeSlider);
	AddUISlider(musicVolumeSlider);
	AddUISlider(SFXVolumeSlider);

	AddUIElement(res960);
	AddUIElement(res1280);
	AddUIElement(res1600);
	AddUIElement(res1920);
	AddUIElement(res2560);

}

StateSettings::~StateSettings()
{
	for (auto& element : myUIElements)
	{
		delete element;
	}
	myUIElements.clear();
}

bool StateSettings::Update()
{
	UpdateUIElements();
	if (Input::GetInstance().GetInputPressed(eButtonInput::Escape))
	{
		Singleton<UIManager>().GetStateStack().Pop();
	}
	return true;
}

void StateSettings::Render()
{
	for (auto& element : myUIElements)
	{
		element->Render();
	}
}
