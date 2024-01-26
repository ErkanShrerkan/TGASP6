#pragma once
#include "State.h"
#include <array>
#include <memory>
#include "Observer.h"
namespace SE
{
    class CScene;
}
class UIElement;
class Stopwatch;
class UIGauge;
class StateGameplay :
    public State,
    public Observer
{
public:
    StateGameplay();
    ~StateGameplay();
    bool Update() override;
    void Render() override;
    void LevelUpRender();
    void SetHpVariable(float* aVariable);
    void SetApVariable(float* aVariable);
    void CreateBossGauge(float* aVariable);
    void DestroyBossGauge();
    void CreateHPBars();
    void CreateXPBars();
    void AddUIOverlays();

    void SetObjectiveIndex(int anIndex) { myObjectiveIndex = anIndex; }
    UIGauge* GetHPGauge();
    UIGauge* GetAPGauge();
    std::array<UIElement*, 7>& GetAbilityElements() { return myAbilityIcons; }
    void SetPlayerAbilityLockedStatus(bool aState, int anIndex);
    void RenderHPBarAt(Vector3f aPosition, float aHealth);
    void RenderXPBar(float anXP);
    void RenderCurrentLevel();
    void SetCurrentLevel(int aLevel);
    void RecieveMessage(eMessage aMsg);



    void ChangeToMaxLevelUI();

private:
    struct EnemyHP
    {
        std::shared_ptr<UIElement> Gauge;
        std::shared_ptr<UIElement> Background;
        Vector2f Position;
    };
    struct PlayerXP
    {
        std::shared_ptr<UIElement> Gauge;
        std::shared_ptr<UIElement> MaxGauge;
        std::shared_ptr<UIElement> currentLevelSprite;
        std::shared_ptr<UIElement> currentLevelSpriteOutline;
        int currentLevel;
    };
    std::array<EnemyHP, 256> myHPBarPool;
    int myHpBarIndex = 0;
    PlayerXP myPlayerXP;
    std::array<UIElement*, 7> myAbilityIcons;
    std::array<UIElement*, 7> myAbilityLocks;
    std::array<UIElement*, 4> myAbilityLockLevels;
    std::array<UIElement*, 7> myAbilityTooltips;
    std::array<UIElement*, 8> myObjectives;
    int myObjectiveIndex = 0;
    float* myHp;
    float* myAp;
    UIElement* myAbilityBar;
    UIElement* myMaxAbilityBar;

    UIElement* myResourceOverlay;
    UIGauge* myHpGauge;
    UIGauge* myApGauge;
    UIGauge* myBossGauge;
    UIElement* myBossGaugeBackground;
    UIElement* myBossGaugeOverlay;
    UIElement* myBossName;

    UIElement* myLevelUpText;
    UIElement* myLevelUpGlow;
    Stopwatch* myLevelUpTimer;
    bool myShouldRenderLevelUpText = false;
    bool myShouldRenderLevelUpGlow = false;
    bool myShouldRenderBossHp = false;
    float myFadeInTimer = 0;
    float myFadeInTimerMax = 0.25f;
    float myFadeOutTimer = 0;
    float myFadeOutTimerMax = 1.0f;
    float myRenderTimer = 0;
    float myRenderTimerMax = 2;




    float myXPPosX = 0.36f;
    float myXPPosY = 0.908f;
};

