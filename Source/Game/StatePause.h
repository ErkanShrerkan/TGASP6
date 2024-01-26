#pragma once
#include "State.h"
#include <vector>
namespace SE
{
    class CGameObject;
}
class UIElement;
class StatePause :
    public State
{
public:
    StatePause();
    ~StatePause();
    bool Update() override;
    void Render() override;
};

