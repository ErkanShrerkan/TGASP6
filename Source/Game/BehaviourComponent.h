#pragma once
#include  <Engine/Component.h>
class BehaviourComponent :
    public SE::CComponent
{
public:
    BehaviourComponent(SE::CGameObject&);
    ~BehaviourComponent();
    void Init() override;
    void Update() override;

};

