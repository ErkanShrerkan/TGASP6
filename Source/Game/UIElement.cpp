#include "pch.h"
#include "UIElement.h"
#include <Engine/Sprite.h>
#include <Engine/DX11.h>
#include <Engine/ContentLoader.h>
#include "Engine/Input.h"
#include "Postmaster.h"
#include "UIManager.h"
UIElement::UIElement(const char* aPath)
{
    mySprite = SE::DX11::Content->GetSpriteFactory().GetSprite(aPath);
}

UIElement::~UIElement()
{
    mySprite->Release();
}

void UIElement::Update()
{
    
}

void UIElement::Render()
{
    mySprite->Render();
}

void UIElement::SetPosition(const Vector2f& aPosition)
{
    mySprite->SetPosition(aPosition);
}

void UIElement::SetPivot(const Vector2f& aPivot)
{
    mySprite->SetPivot(aPivot);
}

void UIElement::SetSize(const Vector2f& aSize)
{
    mySprite->SetSize(aSize);
}

bool UIElement::MouseHovering()
{
    Vector2f mousePos = Input::GetMousePos();

    if (mousePos.x > mySprite->GetPosition().x - (mySprite->GetSize().x * mySprite->GetPivot().x) && 
        mousePos.x < mySprite->GetPosition().x + mySprite->GetNormalizedImageSize().x - (mySprite->GetSize().x * mySprite->GetPivot().x) &&
        mousePos.y > mySprite->GetPosition().y - (mySprite->GetSize().y * mySprite->GetPivot().y) &&
        mousePos.y < mySprite->GetPosition().y + mySprite->GetNormalizedImageSize().y - (mySprite->GetSize().y * mySprite->GetPivot().y))
    {
        return true;
    }
    return false;
}

bool UIElement::GetGreyedOut()
{
    return myShouldRenderLock;
}


const Vector2f UIElement::GetPosition()
{
    return mySprite->GetPosition();
}

const Vector2f& UIElement::GetSize()
{
    return mySprite->GetSize();
}

SE::CSprite* UIElement::GetSprite()
{
    return mySprite;
}
