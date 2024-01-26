#pragma once
#include <Engine/Component.h>
class CSprite;
class SpriteComponent :
    public SE::CComponent
{
public:
    SpriteComponent(SE::CGameObject&);
    ~SpriteComponent();
    void Init(const char* myPath);
    void Update() override;
    void SetPosition(const Vector2f& aPosition);
    void SetSize(const Vector2f& aSize);
    const Vector2f& GetPosition();
    const Vector2f& GetSize();
private:
    CSprite* mySprite;
};

