#include "pch.h"
#include "StateCredits.h"
#include "UIElement.h"
#include <Engine/Input.h>
#include "Postmaster.h"
StateCredits::StateCredits()
{
    UIElement* credits = new UIElement("Textures/Sprites/UI/UI_S_CreditsScreen.dds");
    credits->SetPosition({ 0.5f, 0.5f });
    credits->SetPivot({ 0.5f, 0.5f });
    AddUIElement(credits);
}

StateCredits::~StateCredits()
{
}

bool StateCredits::Update()
{
    for (auto& element : myUIElements)
    {
        element->Update();
    }

    if (Input::GetInputPressed(eButtonInput::Escape))
    {
        Postmaster::GetInstance()->SendMail(eMessage::ePopStack);
    }
    return true;
}

void StateCredits::Render()
{
    for (auto& element : myUIElements)
    {
        element->Render();
    }
}
