#include "pch.h"
#include "UISlider.h"
#include <Engine\DX11.h>
#include <Engine\SpriteFactory.h>
#include <Engine/Sprite.h>
#include <Engine/ContentLoader.h>
#include <Engine\Input.h>
#include <Engine/AudioEngine.h>
#include "UIGauge.h"
UISlider::UISlider(const char* aPath, eMessage aMsgType)
	: UIElement(aPath)
{
	std::string sliderIcon = aPath;
	std::string filename(sliderIcon.begin(), sliderIcon.end() - 14);
	filename = filename + "Icon.dds";
	mySliderIcon = SE::DX11::Content->GetSpriteFactory().GetSprite(filename);
	myMessage = aMsgType;
	myIconHeld = false;
	myValue = 1.f;
	
	myFill = new UIGauge("Textures/Sprites/UI/UI_S_SliderBar.dds", &myValue, 1.f, true, true);
}

void UISlider::Update()
{
	myFill->Update();

	auto increment = 1.f / (myMax - myMin);

	if (Input::GetInputPressed(eButtonInput::LBUTTON) && MouseHoveringIcon())
	{
		myIconHeld = true;
	}
	if (myIconHeld)
	{
		auto mPos = Input::GetMousePos();
		mPos -= mySliderIcon->GetSize() * 0.5f;

		if (mPos.x < myMin)
		{
			mPos.x = myMin;
		}
		if (mPos.x > myMax)
		{
			mPos.x = myMax;
		}
		mySliderIcon->SetPosition({ mPos.x, mySliderIcon->GetPosition().y});

		myValue = mySliderIcon->GetPosition().x - myMin;
		myValue *= increment;
		pout("%f", myValue);

		switch (myMessage)
		{
		case eMessage::eChangeMasterVolume:
			SE::CAudioEngine::GetInstance()->SetBusVolume(AudioBus::SemiMaster, myValue);
			SE::CAudioEngine::GetInstance()->SetBusVolume(AudioBus::Master_Bus, myValue);
			break;
		case eMessage::eChangeMusicVolume:
			SE::CAudioEngine::GetInstance()->SetBusVolume(AudioBus::SemiMaster_Music, myValue);
			break;
		case eMessage::eChangeSFXVolume:
			SE::CAudioEngine::GetInstance()->SetBusVolume(AudioBus::SemiMaster_SFX, myValue);
			break;
		}
	}
	if (Input::GetInputReleased(eButtonInput::LBUTTON))
	{
		myIconHeld = false;
	}

	
}

void UISlider::Render()
{
	mySprite->Render();
	myFill->Render();
	mySliderIcon->Render();
}

void UISlider::SetPosition(const Vector2f& aPosition)
{
	mySprite->SetPosition({ aPosition.x - myFill->GetSprite()->GetSize().x * 0.5f, aPosition.y });
	myFill->SetPosition({ aPosition.x - myFill->GetSprite()->GetSize().x * 0.5f, aPosition.y });
	mySliderIcon->SetPosition({ aPosition.x, aPosition.y - 0.06f });
}

void UISlider::InitWithValue(float aNormalizedValue)
{
	myMin = mySprite->GetPosition().x + 0.03125f;
	myMax = mySprite->GetPosition().x + mySprite->GetSize().x - mySliderIcon->GetSize().x - 0.03125f;

	myValue = aNormalizedValue;
	float vaal = myMax - myMin;
	//vaal = Remap(vaal, myMin, myMax, myMin , myMax );
	float toScreen = (vaal) * aNormalizedValue;
	toScreen += vaal / 2.f;
	mySliderIcon->SetPosition({ aNormalizedValue - mySprite->GetPosition().x, mySliderIcon->GetPosition().y });
}


void UISlider::SetPivot(const Vector2f& aPivot)
{
	mySprite->SetPivot(aPivot);
}

bool UISlider::MouseHoveringIcon()
{
	Vector2f mousePos = Input::GetMousePos();

	if (mousePos.x > mySliderIcon->GetPosition().x && mousePos.x < mySliderIcon->GetPosition().x + mySliderIcon->GetSize().x &&
		mousePos.y > mySliderIcon->GetPosition().y && mousePos.y < mySliderIcon->GetPosition().y + mySliderIcon->GetSize().y)
	{
		return true;
	}
	return false;
}

SE::CSprite* UISlider::GetIcon()
{
	return mySliderIcon;
}
