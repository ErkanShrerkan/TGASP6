#pragma once
#include "TriggerListener.h"
#include "Health.h"
#include "Transform.h"
#include <map>
#include <array>
#include "AudioSystem.h"
#include <Engine/Stopwatch.h>
class AudioComponent;
class TriggerSystem;
class Coordinator;
class PlayerSystem;
struct Shape;

enum class BossState
{
    Idle, 
    FirstAttack,
    SecondAttack,
    ThirdAttack,
    Moving,
    Dead,
    Count
};
class BossSystem : public TriggerListener
{
public:
    BossSystem();
    ~BossSystem();
    void Init(Coordinator& aCoordinator, PlayerSystem* aPlayerSystem);
    void SpawnBoss();
    void SetNpcReference(Entity anNpc);
    void Update(TriggerSystem& aTriggerSystem);

    Entity GetEntity();
private:
    void OnTriggerEnter(const TriggerData& someTriggerData) override;
    void ToggleNewState();

    void UpdateIdle();
    void UpdateFirstAttack(TriggerSystem& aTriggerSystem);
    void UpdateSecondAttack(TriggerSystem& aTriggerSystem);
    void UpdateThirdAttack(TriggerSystem& aTriggerSystem);

    void UpdateSecondAttackComets();

    void StartSecondPhase();
    void StartThirdPhase();

    void MoveBoss();

    void LookAtTarget();
    void UpdateRotationAngle();
    void MovePlayerTowardsBoss();

    void CheckForPhaseShift();

    void LoadAttackOnTargetPosition();
    void CreateTriggersForSecondAttack(TriggerSystem& aTriggerSystem);


    bool IsClockwise(Vector2f aVec1, Vector2f aVec2) const;
    bool IsWithinRadius(Vector2f aVec, float aRadSqr) const;
    bool IsConeTouchingSphere(
        Shape& aShapeA, Transform& aTransformA,
        Transform& aTransformB
    ) const;
private:
    std::map<BossState, BossState> myTransitions = {
        {BossState::Idle, BossState::FirstAttack}
    };


    Entity myBossEntity;
    Entity myTrigger;
    Entity myNpcEntity;
    BossState myCurrentState;
    Vector2f myLookDirection;
    float mySpeed;
    float myStartHpAmount;
    float myIdleTime = 1.f;
    float myFirstAttackChargeTime = 2.f;
    float mySecondAttackFrequencyTime = 1.f;
    float mySecondAttackDelayTime = 0.5f;
    float myThirdAttackDamageFrequency = 0.1f;
    float myMovePositionTime = 1.f;
    float myCurrentFrameAngle;
    float myLerpRotate;
    float myColliderRadius;

    float mySecondPhaseThreshhold;
    float myThirdPhaseThreshhold;

    float myFirstAttackRadius;
    float mySecondAttackRadius;
    float myThirdAttackRadius;

    float myFirstAttackDamage;
    float mySecondAttackDamage;
    float myThirdAttackDamage;

    float mySecondAttackCometAmount;
    float myThirdAttackCometAmount;
    unsigned int mySecondAttackCometCount;
    unsigned int myThirdAttackHitCount;
    unsigned int myUnlockedPhases = 1;

    bool mySecondPhase;
    bool myThirdPhase;
    bool isActive = false;
    bool myHasSpawned = false;
    bool myRoarPlayed = false;
    bool myNpcAnimation = false;
    bool myHasChosenMovePos = false;
    bool isTurningLeft = false;
    bool isTurningRight = false;
    bool myRecentlyAttacked = false;
    bool myFirstAttackLockIn = false;
    bool mySecondAttackStarted = false;
    bool myThirdAttackStarted = false;
    bool mySecondAttackLoadup = true;
    bool myLastStateWasMove = false;
    bool myThirdAttackVfxStarted = false;
    bool myIsDead = false;
    bool myUpdateSuck = false;

    Transform* myTargetPosition;

    Transform* myTransform;
    Health* myHpComponent;
    Coordinator* myCoordinator;

    Stopwatch mySpawnBossStopwatch;
    Stopwatch myNpcAnimationStopwatch;
    Stopwatch myIdleStopwatch;
    Stopwatch myPostAttackStopwatch;
    Stopwatch myFirstStopwatch;
    Stopwatch mySecondStopwatch;
    Stopwatch myThirdStopwatch;
    Stopwatch myMoveLocationStopwatch;
    Stopwatch mySecondAttackLoadupStopwatch;
    Stopwatch myThirdAttackLoadupStopwatch;
    Stopwatch myBossDeathStopwatch;
    Stopwatch myBossRoarStopwatch;

    std::array<Stopwatch, 12> mySecondAttackDelayStopwatches;
    std::array<Transform, 12> mySecondAttackPositions;
    std::array<Vector3f, 3> myPossiblePositions;
    struct bruh
    {
        Vector3f Pos;
        float Rad;

    };
    Vector3f* myPosition;
    Vector3f Position;
    float myScale;
    Transform myFirstAttackPosition;

    Vector3f camPosition;
    Vector3f camRotation;
    float camFov;

    AudioSystem* myAudioSystem;
    AudioComponent myIdleAudio;
    PlayerSystem* myPlayerSystem;

};

