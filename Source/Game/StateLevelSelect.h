#pragma once
#include "State.h"
#include <array>
class UIElement;
class StateLevelSelect :
    public State
{
public:
    StateLevelSelect();
    ~StateLevelSelect();
    bool Update() override;
    void Render() override;
};

