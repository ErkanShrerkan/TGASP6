#pragma once
#include "State.h"
#include <vector>
namespace SE
{
    class CGameObject;
}
class UIElement;
class StateCredits :
    public State
{
public:
    StateCredits();
    ~StateCredits();
    bool Update() override;
    void Render() override;
};

