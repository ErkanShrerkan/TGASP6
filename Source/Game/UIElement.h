#pragma once
#include <Engine/Component.h>
namespace SE
{
    class CSprite;
}
class UIElement
{
public:
    UIElement(const char* aPath);
    ~UIElement();
    virtual void Update();
    virtual void Render();
    virtual void SetPosition(const Vector2f& aPosition);
    virtual void SetPivot(const Vector2f& aPivot);
    virtual void SetSize(const Vector2f& aSize);
    virtual bool MouseHovering();
    bool GetGreyedOut();
    const Vector2f GetPosition();
    const Vector2f& GetSize();
    SE::CSprite* GetSprite();
    bool myIsEnemyHp = false;
    bool myAbilityIcon = false;
    bool myAbilityTooltip = false;
    bool myIsLock = false;
    bool myShouldRenderLock = false;
    bool myObjective = false;
    bool myIsBossHp = false;
protected:
    SE::CSprite* mySprite;
};

