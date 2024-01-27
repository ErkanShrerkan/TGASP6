#pragma once
#include "UIElement.h"
#include "eMessageTypes.h"
class UIGauge;
class UISlider :
    public UIElement
{
public:
    ~UISlider();
    UISlider(const char* aPath, eMessage aMsgType);
    void Update() override;
    void Render() override;
    void SetPosition(const Vector2f& aPosition) override;
    void InitWithValue(float aNormalizedValue);
    void SetPivot(const Vector2f& aPivot) override;
    bool MouseHoveringIcon();
    SE::CSprite* GetIcon();
private:
    float Remap(float aValue, float aLow1, float aHigh1, float aLow2, float aHigh2)
    {
        return aLow2 + (aValue - aLow1) * (aHigh2 - aLow2) / (aHigh1 - aLow1);
    }
    eMessage myMessage;
    SE::CSprite* mySliderIcon;
    UIGauge* myFill;
    bool myIconHeld;
    float myValue;
    float myMin, myMax;


};

