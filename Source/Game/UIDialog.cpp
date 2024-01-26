#include "pch.h"
#include "UIDialog.h"
#include <Engine/Sprite.h>

UIDialog::UIDialog(const char* aPath, int aKeyframeStart) : UIElement(aPath), myKeyframeStart(aKeyframeStart)
{
    mySprite->SetPivot({ 0.5f, 0.5f });
    mySprite->SetPosition({ 0.5f, 0.9f });
}

UIDialog::~UIDialog()
{
}

const int UIDialog::GetKeyframe() const
{
    return myKeyframeStart;
}

//void UIDialog::Render()
//{
//}
//
//void UIDialog::Update()
//{
//}
