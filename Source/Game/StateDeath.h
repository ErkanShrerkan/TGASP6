#pragma once
#include "State.h"
#include <vector>
namespace SE
{
    class CGameObject;
}
class UIElement;
class StateDeath :
    public State
{
public:
    StateDeath();
    ~StateDeath();
    bool Update() override;
    void Render() override;
};

