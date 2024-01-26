#pragma once
#include "State.h"
#include <vector>
namespace SE
{
    class CGameObject;
}
class UIElement;
class StateSettings :
    public State
{
public:
    StateSettings();
    ~StateSettings();
    bool Update() override;
    void Render() override;
};

