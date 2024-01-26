#pragma once
#include "UIElement.h"
class UIGauge :
    public UIElement
{
public:
    UIGauge(const char* aPath, float* aValueToGauge, float aMaxValue, bool isRect = false, bool isSlider = false);
    ~UIGauge();
    void Update() override;
    void Render() override;
    void SetSize(const Vector2f& aSize) override;
    void SetNewVariableReference(float* aValueToGauge);
    float& GetVal();

private:
    float Remap(float aValue, float aLow1, float aHigh1, float aLow2, float aHigh2)
    {
        return aLow2 + (aValue - aLow1) * (aHigh2 - aLow2) / (aHigh1 - aLow1);
    }
    float* myValue;
    float ax = 0;
    float ay = 0;
    float time = 0.f;
    float speed = 0.1f;
    float myMaxValue;
    Vector2f myOGSize;
    bool myIsRect;
    bool myIsSlider;
    float myVal;
};

