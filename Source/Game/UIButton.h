#pragma once
#include "UIElement.h"
#include "eMessageTypes.h"
class UIButton :
    public UIElement
{
public:
    UIButton(const char* aPath, bool isBig, eMessage aMsgType, bool isLevelSelect = false);
    void Update() override;
    void Render() override;
    bool MouseHovering() override;
    void SetPosition(const Vector2f& aPosition) override;
    void SetSize(const Vector2f& aPosition) override;
private:
    eMessage myMessage;
    UIElement* myBackground;
    UIElement* myHighlightedBackground;
    bool myIsBig;
    bool myIsLevelSelect;
};

