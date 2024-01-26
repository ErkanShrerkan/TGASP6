#pragma once
#include <Engine/Component.h>
class CSprite;
class NavmeshAgentComponent :
    public SE::CComponent
{
public:
    NavmeshAgentComponent(SE::CGameObject&);
    ~NavmeshAgentComponent();
    void Init() override;
    void Update() override;

private:

};

