#include "pch.h"
#include "State.h"
#include "UIManager.h"
#include "UIElement.h"
#include "UISlider.h"
void State::AddUIElement(UIElement* aUIElement)
{
    myUIElements.push_back(aUIElement);
    Singleton<UIManager>().AddUIElement(aUIElement);
}

void State::AddUISlider(UISlider* aUISlider)
{
    myUIElements.push_back(aUISlider);
    Singleton<UIManager>().AddUISlider(aUISlider);
}

void State::UpdateUIElements()
{
    for (auto& element : myUIElements)
    {
        element->Update();
    }
}

eStateID State::GetStateID() const
{
    return myStateID;
}

bool State::operator==(const State& aStateID)
{
    if (myStateID == aStateID.GetStateID())
    {
        return true;
    }
    return false;
}
