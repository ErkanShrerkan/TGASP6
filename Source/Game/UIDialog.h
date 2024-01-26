#pragma once
#include "UIElement.h"
class UIDialog :
    public UIElement
{

public:
    UIDialog(const char* aPath, int aKeyframeStart);
    ~UIDialog();
    //void Render() override;
    //void Update() override;
    const int GetKeyframe() const;
private:
    const int myKeyframeStart;
};



