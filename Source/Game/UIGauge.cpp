#include "pch.h"
#include "UIGauge.h"
#include <Engine\Sprite.h>
#include <Engine\Engine.h>
UIGauge::UIGauge(const char* aPath, float* aValueToGauge, float aMaxValue, bool isRect, bool isSlider) :UIElement(aPath)
{
	myMaxValue = aMaxValue;
	myIsRect = isRect;
	myIsSlider = isSlider;
	myValue = aValueToGauge;
	if (myIsRect)
	{
		mySprite->SetMask("Textures/MaskDefault.dds");

	}
	else
	{
		mySprite->SetMask("Textures/MaskCircle.dds");
		mySprite->SetSizeRelativeToImage({ 0.33f, 0.33f });
		mySprite->SetShaderType(SE::SpriteShaderType::eFisheye);
	}

}

UIGauge::~UIGauge()
{
}

void UIGauge::Update()
{
	if (myValue != nullptr)
	{
		float dt = SE::CEngine::GetInstance()->GetDeltaTime();
		time += dt;
		if (myIsRect && !myIsSlider)
		{
			mySprite->SetRect({ 0.f, 0.f, *myValue / myMaxValue, 1.f });

			mySprite->SetSizeRelativeToImage(Vector2f((*myValue / myMaxValue), 1.f));
		}
		else if (myIsSlider)
		{
			myVal = Remap(*myValue / myMaxValue, 0.f, 1.f, 0.09765f + 0.015625f, 1.f - 0.09765f - 0.015625f);
			mySprite->SetRect({ 0.f, 0.f, myVal, 1.f });

			mySprite->SetSizeRelativeToImage(Vector2f((myVal), 1.f));
		}
		else
		{
			if (time >= 3.14f * 10.f)
			{
				time = 0.f;
			}
			ax += dt * speed;

			if (ax > 0.5f)
			{
				ax = 0.f;
			}
			ay = sinf(time) * 0.15f;

			float myValueDividedByMaxValue = *myValue / myMaxValue;
			float newValue = Remap(myValueDividedByMaxValue, 0, 1, 0, 0.5f);

				//newValue = Remap(*myValue / myMaxValue, 0, 1, 0, 0.5f);

			mySprite->SetRect({ 0.f + ax, /**myValue * 0.0005f*/newValue + 0.1f * (1 - myValueDividedByMaxValue), 0.5f + ax, 0.5f + /**myValue * 0.0005f*/newValue + 0.1f * (1 - myValueDividedByMaxValue)});
		}
	}


}

void UIGauge::Render()
{
	mySprite->Render();
}

void UIGauge::SetSize(const Vector2f& aSize)
{
	mySprite->SetSize(aSize);
	myOGSize = aSize;
}

void UIGauge::SetNewVariableReference(float* aValueToGauge)
{
	myValue = aValueToGauge;
}

float& UIGauge::GetVal()
{
	return myVal;
}
