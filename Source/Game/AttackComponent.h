#pragma once
#include <Engine/Component.h>
class AttackComponent :
    public SE::CComponent
{
public:
    AttackComponent(SE::CGameObject&);
    ~AttackComponent();
    void Update() override;
    void Init(int aAttackValue);
    void Attack();

private:
    int myAttackValue;
    int myCriticalStrikeChance;
};

