#pragma once
#include <Engine/Component.h>
class HealthComponent :
    public SE::CComponent
{
public:
    HealthComponent(SE::CGameObject&);
    ~HealthComponent();
    void Init(int aHealthAmount);
    void Update() override;
    const int GetHealth();
    const bool IsDead();
private:
    int myHealth;
    
};

