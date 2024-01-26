#pragma once
#include "State.h"
#include <array>
class UIElement;
class StateMainMenu :
    public State
{
public:
    StateMainMenu();
    ~StateMainMenu();
    bool Update() override;
    void Render() override;
    void SetCamPos(float3 aPos, int aIndex);
    Vector3f GetPosByIndex(int aIndex);
private:
    std::array<float3, 5> myCameraPositions;
};

