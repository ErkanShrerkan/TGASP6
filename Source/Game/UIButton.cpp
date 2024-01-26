#include "pch.h"
#include "UIButton.h"
#include "Postmaster.h"
#include <Engine/Input.h>
#include <Engine\Sprite.h>
#include "Engine\AudioEngine.h"
UIButton::UIButton(const char* aPath, bool isBig, eMessage aMsgType, bool isLevelSelect)
	: UIElement(aPath)
{
    myMessage = aMsgType;
    myIsBig = isBig;
    myIsLevelSelect = isLevelSelect;
    const char* buttonType;
    const char* highlightedbuttonType;
    if(isBig)
    {
        buttonType = "Textures/Sprites/UI/UI_S_BigButton.dds";
        highlightedbuttonType = "Textures/Sprites/UI/UI_S_BigButtonHighlight.dds";
    }
    else
    {
        buttonType = "Textures/Sprites/UI/UI_S_SmallButton.dds";
        highlightedbuttonType = "Textures/Sprites/UI/UI_S_SmallButtonHighlight.dds";
    }
    myBackground = new UIElement(buttonType);
    myHighlightedBackground = new UIElement(highlightedbuttonType);

}

void UIButton::Update()
{
    //grey out highlighted item
    bool hovering = MouseHovering();

    if (Input::GetInstance().GetInputPressed(eButtonInput::LBUTTON))
    {
        if (hovering)
        {
            Postmaster::GetInstance()->SendMail(myMessage);
            SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::UI_ButtonClick);

        }
    }
}

void UIButton::Render()
{
    if (MouseHovering())
    {
        myHighlightedBackground->Render();
    }
    else
    {
        myBackground->Render();
    }
    mySprite->Render();
}

bool UIButton::MouseHovering()
{
    Vector2f mousePos = Input::GetMousePos();
    auto s = myBackground->GetSprite();
    if (mousePos.x > s->GetPosition().x && mousePos.x < s->GetPosition().x + s->GetSize().x &&
        mousePos.y > s->GetPosition().y && mousePos.y < s->GetPosition().y + s->GetSize().y)
    {
        return true;
    }
    return false;
}

void UIButton::SetPosition(const Vector2f& aPosition)
{
    if (myIsBig)
    {
        mySprite->SetPosition({ aPosition.x - 0.12f, aPosition.y + 0.029f});
    }
    else if (myIsLevelSelect)
    {
        mySprite->SetPosition({ aPosition.x - 0.095f, aPosition.y + 0.029f });
    }
    else
    {
        mySprite->SetPosition({ aPosition.x - 0.13f, aPosition.y + 0.027f });
    }
    myBackground->SetPosition(aPosition);
    myHighlightedBackground->SetPosition(aPosition);
}

void UIButton::SetSize(const Vector2f& aSize)
{
    aSize;
    //mySprite->SetSizeRelativeToImage(aSize);
    //myGreyoutSprite->SetSizeRelativeToImage(aSize);
    myBackground->GetSprite()->SetSizeRelativeToImage(aSize);
    myHighlightedBackground->GetSprite()->SetSizeRelativeToImage(aSize);
}
