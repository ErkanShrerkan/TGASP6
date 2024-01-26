#pragma once
#include <Engine/Component.h>
class CSprite;
class SpawnComponent :
    public SE::CComponent
{
public:
    SpawnComponent(SE::CGameObject&);
    ~SpawnComponent();
    void Init() override;
    void Update() override;
    void Spawn();

private:
    
};

