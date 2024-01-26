#include "pch.h"
#include "BossSystem.h"
#include "Coordinator.h"
#include "Collider.h"
#include "Engine\DX11.h"
#include "Engine\ContentLoader.h"
#include <Engine\Input.h>
#include "TriggerSystem.h"
#include "Engine\Stopwatch.h"
#include <Engine\Engine.h>
#include "ModelCollection.h"
#include "Expose.h"
#include "Postmaster.h"
#include "StateGameplay.h"
#include "StateStack.h"
#include "GameWorld.h"
#include <Engine\Camera.h>
#include "UIManager.h"
#include "PlayerSystem.h"
#include <Engine\CDebugDrawer.h>
#include <Engine/Animator.h>
#include <Engine/VFXManager.h>
#include "StateCutscene.h"
BossSystem::BossSystem()
{
    myCoordinator = nullptr;
    myHpComponent = nullptr;
    myTransform = nullptr;
}

BossSystem::~BossSystem()
{

    myCoordinator =  nullptr;
    myHpComponent =  nullptr;
    myTransform =  nullptr;
    myPosition = nullptr;

    UNEXPOSE(mySpeed);
    UNEXPOSE(myStartHpAmount);
    UNEXPOSE(mySecondPhaseThreshhold);
    UNEXPOSE(myThirdPhaseThreshhold);
    UNEXPOSE(mySecondPhase);
    UNEXPOSE(myThirdPhase);
    UNEXPOSE(myFirstAttackRadius);
    UNEXPOSE(mySecondAttackRadius);
    UNEXPOSE(myThirdAttackRadius);
    UNEXPOSE(myFirstAttackDamage);
    UNEXPOSE(mySecondAttackDamage);
    UNEXPOSE(myThirdAttackDamage);
    UNEXPOSE(mySecondAttackCometAmount);
    UNEXPOSE(myThirdAttackCometAmount);
    UNEXPOSE(myIdleTime);
    UNEXPOSE(myFirstAttackChargeTime);
    UNEXPOSE(mySecondAttackFrequencyTime);
    UNEXPOSE(mySecondAttackDelayTime);
    UNEXPOSE(myThirdAttackDamageFrequency);
    UNEXPOSE(myMovePositionTime);
    UNEXPOSE(myColliderRadius);
    UNEXPOSE(camPosition);
    UNEXPOSE(camRotation);
    UNEXPOSE(camFov);

    StateGameplay* gameplay = reinterpret_cast<StateGameplay*>(&(Singleton<UIManager>().GetStateStack().GetStateFromId(eStateID::Playing)));
    gameplay->DestroyBossGauge();
}

void BossSystem::Init(Coordinator& aCoordinator, PlayerSystem* aPlayerSystem)
{
    EXPOSE(mySpeed);
    EXPOSE(myStartHpAmount);
    EXPOSE(mySecondPhaseThreshhold);
    EXPOSE(myThirdPhaseThreshhold);
    EXPOSE(mySecondPhase);
    EXPOSE(myThirdPhase);
    EXPOSE(myFirstAttackRadius);
    EXPOSE(mySecondAttackRadius);
    EXPOSE(myThirdAttackRadius);
    EXPOSE(myFirstAttackDamage);
    EXPOSE(mySecondAttackDamage);
    EXPOSE(myThirdAttackDamage);
    EXPOSE(mySecondAttackCometAmount);
    EXPOSE(myThirdAttackCometAmount);
    EXPOSE(myIdleTime);
    EXPOSE(myFirstAttackChargeTime);
    EXPOSE(mySecondAttackFrequencyTime);
    EXPOSE(mySecondAttackDelayTime);
    EXPOSE(myThirdAttackDamageFrequency);
    EXPOSE(myMovePositionTime);
    EXPOSE(myColliderRadius);
    EXPOSE(camPosition);
    EXPOSE(camRotation);
    EXPOSE(camFov);

    myIdleStopwatch = Stopwatch(9.f);
    myFirstStopwatch =  Stopwatch(myFirstAttackChargeTime);
    mySecondStopwatch =  Stopwatch(mySecondAttackFrequencyTime);
    myThirdStopwatch =  Stopwatch(myThirdAttackDamageFrequency);
    myMoveLocationStopwatch =  Stopwatch(myMovePositionTime);
    mySecondAttackLoadupStopwatch =  Stopwatch(2.f);
    myThirdAttackLoadupStopwatch =  Stopwatch(2.f);
    myPostAttackStopwatch =  Stopwatch(1.f);
    mySpawnBossStopwatch =  Stopwatch(12.f);
    myNpcAnimationStopwatch =  Stopwatch(7.f);
    myBossDeathStopwatch =  Stopwatch(3.f);
    myBossRoarStopwatch =  Stopwatch(4.f);
    myIdleStopwatch     .Reset();
    myFirstStopwatch    .Reset();
    mySecondStopwatch   .Reset();
    myThirdStopwatch    .Reset();
    myMoveLocationStopwatch.Reset();
    myThirdAttackHitCount = 0;
    mySecondAttackCometCount = 0;
    for (size_t i = 0; i < mySecondAttackDelayStopwatches.size(); i++)
    {
        mySecondAttackDelayStopwatches[i] = Stopwatch(mySecondAttackDelayTime);
        mySecondAttackDelayStopwatches[i].Reset();
    }

    myCoordinator = &aCoordinator;
    myBossEntity = aCoordinator.CreateEntity();
    //index 0 is player
    myTargetPosition = &aCoordinator.GetComponent<Transform>(0);

    Health health;
    health.Init(myStartHpAmount);
    aCoordinator.AddComponent(myBossEntity, health);
    myHpComponent = &aCoordinator.GetComponent<Health>(myBossEntity);


    Collider collider;
    collider.shape.asSphere.radius = myColliderRadius;
    collider.shape.type = ShapeType::Sphere;
    collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_AOE);
    collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_RmbAttack);
    collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Lmb);
    collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Ultimate);
    aCoordinator.AddComponent(myBossEntity, collider);

    Transform transform;
    aCoordinator.AddComponent(myBossEntity, transform);
    myTransform = &aCoordinator.GetComponent<Transform>(myBossEntity);

    auto mod = SE::DX11::Content->GetModelFactory().GetModel("Models/CH_EY_Boss/CH_EY_Boss.erc");
    ModelCollection model{ mod , new SE::CAnimator(mod)};
    model.animator->AddAnimation("Models/CH_EY_Boss/CH_EY_Boss_Idle_AN.myr", "Idle");
    model.animator->AddAnimation("Models/CH_EY_Boss/CH_EY_Boss_Appears_AN.myr", "Appear");
    model.animator->AddAnimation("Models/CH_EY_Boss/CH_EY_Boss_Attack_One_AN.myr", "A1");
    model.animator->AddAnimation("Models/CH_EY_Boss/CH_EY_Boss_Attack_Two_AN.myr", "A2");
    model.animator->AddAnimation("Models/CH_EY_Boss/CH_EY_Boss_Attack_three_AN.myr", "A3");
    model.animator->AddAnimation("Models/CH_EY_Boss/CH_EY_Boss_Move_Left_AN.myr", "Left");
    model.animator->AddAnimation("Models/CH_EY_Boss/CH_EY_Boss_Move_Right_AN.myr", "Right");
    model.animator->AddAnimation("Models/CH_EY_Boss/CH_EY_Boss_Dives_AN.myr", "Dive");
    model.animator->AddAnimation("Models/CH_EY_Boss/CH_EY_Boss_Death_AN.myr", "Death");

    model.animator->SetFallbackAnimation("Idle");
    aCoordinator.AddComponent(myBossEntity, model);

    myCurrentState = BossState::Count;
    mySecondPhase = false;
    myThirdPhase = false;

    myPosition = new Vector3f();

    myPlayerSystem = aPlayerSystem;

    myAudioSystem = AudioSystem::GetInstance();
    myIdleAudio.SetAudioClip(AudioClip::Enemies_Boss_Idle);
}

void BossSystem::SpawnBoss()
{
    myPossiblePositions[0] = { 7750.f, 450.f, -1700.f };
    myPossiblePositions[1] = { 5800.f, 450.f, -575.f };
    myPossiblePositions[2] = { 5800.f, 450.f, -2825.f };

    myPosition = &myPossiblePositions[0];

    myIsDead = false;
    myHasSpawned = true;
    //Do preparatory stuff for boss, like play anims and whatevaah
    StateGameplay* myGameplay = reinterpret_cast<StateGameplay*>(&(Singleton<UIManager>().GetStateStack().GetStateFromId(eStateID::Playing)));
    myGameplay->CreateBossGauge(&myHpComponent->GetHealth());

    myCurrentState = BossState::Idle;
    myCoordinator->GetComponent<ModelCollection>(myBossEntity).animator->Play("Appear", false, "Idle", false);


    


    myTransform->SetPosition(*myPosition);
    myTransform->SetScale({ 1.1f,1.1f,1.1f });


    SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Boss_Intro, *myPosition, true);
    myIdleAudio.Start();


}

void BossSystem::SetNpcReference(Entity anNpc)
{
    myNpcEntity = anNpc;
}

void BossSystem::Update(TriggerSystem& aTriggerSystem)
{
    myIdleAudio.Update(myTransform->GetPosition(), true);
   /* float bossHealth = myHpComponent->GetHealth() / myStartHpAmount * 100.0f;
    myAudioSystem->UpdateMusicParameter("BossHealth", bossHealth);*/
    float playerHealth = myPlayerSystem->GetHealthComponent()->GetHealth() / 1000 * 100.0f;
    myAudioSystem->UpdateMusicParameter("PlayerHealth", playerHealth);
    
    if (myCurrentState != BossState::Count)
    {
        GameWorld::GetInstance()->GetCamera()->SetRotation(camRotation);
        GameWorld::GetInstance()->GetCamera()->SetPosition(camPosition);
    }
    if (isActive && myHasSpawned == false)
    {
        mySpawnBossStopwatch.Update(ENGINE->GetDeltaTime(), false);
        myNpcAnimationStopwatch.Update(ENGINE->GetDeltaTime(), false);
    }
    if (myNpcAnimation && myNpcAnimationStopwatch.IsOver())
    {
        myCoordinator->GetComponent<Transform>(myNpcEntity).SetPosition({ 0.f, -999.f, 0.f });
    }
    if (myNpcAnimation == false && myNpcAnimationStopwatch.IsOver())
    {
        myCoordinator->GetComponent<ModelCollection>(myNpcEntity).animator->Play("Jump", false);
        SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Cultist_Jump);
        myNpcAnimationStopwatch.Reset();
        myNpcAnimationStopwatch.SetTimeInterval(1.9f);
        myNpcAnimation = true;
    }
    
    if (myHasSpawned == false && mySpawnBossStopwatch.IsOver())
    {
        CAMERA->Shake(7.f, 4.f);

        SpawnBoss();
    }
    if (myHasSpawned)
    {
        myBossRoarStopwatch.Update(ENGINE->GetDeltaTime(), false);
        if (myRoarPlayed == false && myBossRoarStopwatch.IsOver())
        {
            myRoarPlayed = true;
            CAMERA->Shake(35.f, 2.f);
        }
    }
    if (myHpComponent->GetHealth() <= 1.f)
    {

        if (myCurrentState != BossState::Dead)
        {
            myCoordinator->GetComponent<ModelCollection>(myBossEntity).animator->Play("Death", false);
            SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Boss_Death, myTransform->GetPosition(), true);
            myIdleAudio.Stop();
            myAudioSystem->StopGameAmbience();
            myAudioSystem->UpdateMusicParameter("BossHealth", 0.0f);
            SE::CAudioEngine::GetInstance()->SetMuteBus(AudioBus::SemiMaster_SFX_InGameSFX_Player, true);
        }
        //myHpComponent->SetHealth(myStartHpAmount);
        myCurrentState = BossState::Dead;
    }

    if (myRecentlyAttacked == true)
    {
        myPostAttackStopwatch.Update(ENGINE->GetDeltaTime(), false);
    }

    switch (myCurrentState)
    {
    case BossState::Idle:
        UpdateIdle();
        break;
    case BossState::FirstAttack:
        UpdateFirstAttack(aTriggerSystem);
        break;
    case BossState::SecondAttack:
        UpdateSecondAttack(aTriggerSystem);
        break;
    case BossState::ThirdAttack:
        UpdateThirdAttack(aTriggerSystem);
        break;
    case BossState::Moving:
        MoveBoss();
        break;
    case BossState::Dead:
        myBossDeathStopwatch.Update(ENGINE->GetDeltaTime(), false);
        if (myBossDeathStopwatch.IsOver() && !myIsDead)
        {
            Singleton<UIManager>().GetStateStack().PushState(eStateID::Cutscene);
            Singleton<UIManager>().GetStateStack().GetCutscene()->PlayCutscene(eCutscenes::BossDead);
            myIsDead = true;
        }
        break;
    default:
        break;
    }

}

Entity BossSystem::GetEntity()
{
    return myBossEntity;
}

void BossSystem::OnTriggerEnter(const TriggerData& someTriggerData)
{
    if (someTriggerData.affectedEntity == myBossEntity &&
        (someTriggerData.event == TriggerEvent::Player_Ability_AOE ||
        someTriggerData.event == TriggerEvent::Player_Ability_RmbAttack ||
        someTriggerData.event == TriggerEvent::Player_Ability_Lmb ||
        someTriggerData.event == TriggerEvent::Player_Ability_Ultimate))
    {
        auto& health = myCoordinator->GetComponent<Health>(someTriggerData.affectedEntity);
        health.TakeDamage(someTriggerData.damage);
    }
    if (!isActive && someTriggerData.affectedEntity == 0 && someTriggerData.event == TriggerEvent::SpawnBoss)
    {
        isActive = true;
    }
}

void BossSystem::ToggleNewState()
{
    int random = rand() % myUnlockedPhases;
    int randomMove = rand() % 4;
    switch (myCurrentState)
    {
    case BossState::Idle:
        if (randomMove == 0 && !myLastStateWasMove)
        {
            myCurrentState = BossState::Moving;
            myLastStateWasMove = true;
            break;
        }
         if (random == 0)
        {
            myCurrentState = BossState::FirstAttack;
            myLastStateWasMove = false;
            break;
        }
        else if (random == 1)
        {
            myCurrentState = BossState::SecondAttack;
            myLastStateWasMove = false; 
            break;
        }
        else if (random == 2)
        {
            myCurrentState = BossState::ThirdAttack;
            myLastStateWasMove = false;
            break;
        }
        break;
    case BossState::FirstAttack:
            myCurrentState = BossState::Idle;
        break;
    case BossState::SecondAttack:
            myCurrentState = BossState::Idle;
        break;
    case BossState::ThirdAttack:
            myCurrentState = BossState::Idle;
        break;
    case BossState::Moving:
        myCurrentState = BossState::Idle;
        break;
    default: __assume(0);
    }
    myRecentlyAttacked = false;
}

void BossSystem::UpdateIdle()
{
    myIdleStopwatch.Update(SE::CEngine::GetInstance()->GetDeltaTime());

    UpdateRotationAngle();

    if (0 == myCurrentFrameAngle * myLerpRotate)
    {
        isTurningRight = false;
        isTurningLeft = false;
    }
    else if (0 < myCurrentFrameAngle * myLerpRotate)
    {
        if (!isTurningLeft && myCoordinator->GetComponent<ModelCollection>(myBossEntity).animator->IsOver())
        {
            isTurningLeft = true;
            isTurningRight = false;
            myCoordinator->GetComponent<ModelCollection>(myBossEntity).animator->Play("Left", false, "Idle");

        }
    }
    else
    {
        if (!isTurningRight && myCoordinator->GetComponent<ModelCollection>(myBossEntity).animator->IsOver())
        {
            isTurningRight = true;
            isTurningLeft = false;
            myCoordinator->GetComponent<ModelCollection>(myBossEntity).animator->Play("Right", false, "Idle");
        }
    }

    //maybe just timer here?
    LookAtTarget();

    CheckForPhaseShift();

    if (myIdleStopwatch.IsOver())
    {
        ToggleNewState();
        myIdleStopwatch.SetTimeInterval(myIdleTime);
        myIdleStopwatch.Reset();
    }
}


void BossSystem::UpdateFirstAttack(TriggerSystem& aTriggerSystem)
{
    if (!myRecentlyAttacked)
    {
        if (!myFirstAttackLockIn)
        {
            SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Boss_Attack1_Channel, *myPosition, true);
            SE::CVFXManager::GetInstance().PlayVFX("BossConeAnticipation", *myTransform);


            myFirstAttackPosition = *myTargetPosition;
            myCoordinator->GetComponent<ModelCollection>(myBossEntity).animator->Play("A1", false, "Idle");
            myFirstAttackLockIn = true;

        }

        myFirstStopwatch.Update(SE::CEngine::GetInstance()->GetDeltaTime());


        Trigger myConeTrigger;
        myConeTrigger.event = TriggerEvent::Boss_FirstAttack;
        myConeTrigger.shape.asCone.radius = myFirstAttackRadius;
        myConeTrigger.shape.asCone.angle = 60.f;

        myConeTrigger.shape.type = ShapeType::Cone;
        myConeTrigger.damage = myFirstAttackDamage;
        float3 distance = (myFirstAttackPosition.GetPosition() - myTransform->GetPosition());
        float2 relPoint = { distance.x, distance.z };
        myConeTrigger.shape.asCone.x = relPoint.x;
        myConeTrigger.shape.asCone.y = relPoint.y;


        Transform transform;
        transform.SetPosition(myTransform->GetPosition());

#ifdef _DEBUG
        std::vector<Transform> spheres;

        for (int i = -16; i < 16; i++)
        {
            for (int j = -16; j < 16; j++)
            {
                float4 variabelnamn = { i * 200.f + myTransform->GetPosition().x, myTransform->GetPosition().y + 500.f, j * 200.f + myTransform->GetPosition().z , 1 };
                Transform transa;
                transa.GetTransform().SetRow(4, variabelnamn);
                spheres.push_back(transa);

            }
        }

        for (auto& sphere : spheres)
        {
            if (IsConeTouchingSphere(myConeTrigger.shape, transform, sphere))
            {
                CDebugDrawer::GetInstance().DrawSphere(sphere.GetTransform().GetRow(4).xyz, 20.f, { 1.f, 0.f, 1.f, 1.f });
            }
        }
#endif // _DEBUG


        if (myFirstStopwatch.IsOver())
        {




            SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Boss_Attack1_Execute, *myPosition, true);

            SE::CVFXManager::GetInstance().PlayVFX("BossConeHit", *myTransform);
            myTrigger = aTriggerSystem.CreateTrigger(myConeTrigger, transform);
            myRecentlyAttacked = true;
        }
    }
    

    if (myPostAttackStopwatch.IsOver())
    {
        myRecentlyAttacked = false;
        myFirstAttackLockIn = false;
        myFirstStopwatch.Reset();
        myPostAttackStopwatch.Reset();
        myPostAttackStopwatch.Update(ENGINE->GetDeltaTime());
        ToggleNewState();
    }
}

void BossSystem::UpdateSecondAttack(TriggerSystem& aTriggerSystem)
{
    if (!myRecentlyAttacked)
    {
        if (!mySecondAttackStarted)
        {
            myCoordinator->GetComponent<ModelCollection>(myBossEntity).animator->Play("A2", false, "Idle");
            SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Boss_CometAttack_FirstChannel, *myPosition, true);

            mySecondAttackStarted = true;
        }

        if (mySecondAttackLoadup)
        {
            if (mySecondAttackLoadupStopwatch.IsOver())
            {
                mySecondAttackLoadup = false;
            }
            mySecondAttackLoadupStopwatch.Update(ENGINE->GetDeltaTime());
            
        }
        else
        {
            mySecondStopwatch.Update(SE::CEngine::GetInstance()->GetDeltaTime());

            if (mySecondStopwatch.IsOver())
            {
                LoadAttackOnTargetPosition();

                mySecondStopwatch.Reset();
            }
            CreateTriggersForSecondAttack(aTriggerSystem);

            UpdateSecondAttackComets();
        }   
    }
    


    
    if (mySecondAttackCometCount > static_cast<unsigned int>(mySecondAttackCometAmount))
    {
        mySecondAttackCometCount = 0;
        for (auto& stopwatch : mySecondAttackDelayStopwatches)
        {
            stopwatch.SetTimeInterval(mySecondAttackDelayTime);
            stopwatch.Reset();
        }

        myRecentlyAttacked = true;
    }
    if (myPostAttackStopwatch.IsOver())
    {
        myRecentlyAttacked = false;
        mySecondAttackStarted = false;
        mySecondAttackLoadup = true;
        myPostAttackStopwatch.Reset();
        myPostAttackStopwatch.Update(ENGINE->GetDeltaTime());
        ToggleNewState();
    }
}
void BossSystem::UpdateSecondAttackComets()
{
    for (unsigned int i = 0; i < mySecondAttackCometCount; i++)
    {
        mySecondAttackDelayStopwatches[i].Update(SE::CEngine::GetInstance()->GetDeltaTime());
    }
}


void BossSystem::LoadAttackOnTargetPosition()
{
    if (mySecondAttackCometCount != static_cast<unsigned int>(mySecondAttackCometAmount))
    {
        SE::CVFXManager::GetInstance().PlayVFX("SpitAnticipation", *myTargetPosition);
        SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Boss_CometAttack_Channel, *myPosition, true);
    }
    mySecondAttackPositions[mySecondAttackCometCount] = *myTargetPosition;
    mySecondAttackCometCount++;
}

void BossSystem::CreateTriggersForSecondAttack(TriggerSystem& aTriggerSystem)
{
    for (unsigned int i = 0; i < mySecondAttackCometCount; i++)
    {
        if (mySecondAttackDelayStopwatches[i].IsOver())
        {
            Trigger myAttackTrigger;
            myAttackTrigger.event = TriggerEvent::Boss_ThirdAttack;
            myAttackTrigger.shape.asSphere.radius = mySecondAttackRadius;

            myAttackTrigger.shape.type = ShapeType::Sphere;
            myAttackTrigger.damage = mySecondAttackDamage;
            SE::CVFXManager::GetInstance().PlayVFX("SpitHit", mySecondAttackPositions[i]);
            aTriggerSystem.CreateTrigger(myAttackTrigger, mySecondAttackPositions[i]);

            SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Boss_CometAttack_Impact, mySecondAttackPositions[i].GetPosition(), true);

            mySecondAttackDelayStopwatches[i].SetTimeInterval(1000.f);
        }
    }
}

bool BossSystem::IsClockwise(Vector2f aVec1, Vector2f aVec2) const
{
    return -aVec1.x * aVec2.y + aVec1.y * aVec2.x > 0;
}

bool BossSystem::IsWithinRadius(Vector2f aVec, float aRadSqr) const
{
    return aVec.x * aVec.x + aVec.y * aVec.y <= aRadSqr;

}

bool BossSystem::IsConeTouchingSphere(Shape& aShapeA, Transform& aTransformA, Transform& aTransformB) const
{
    float length = aShapeA.asCone.radius;
    float angle = aShapeA.asCone.angle / 2.f;

    float2 dir = { aTransformB.GetPosition().x - aTransformA.GetPosition().x, aTransformB.GetPosition().z - aTransformA.GetPosition().z };
 
    float2 re = *reinterpret_cast<float2*>(&aShapeA.asCone.x);

    re.Normalize();

    float radian = acosf(dir.GetNormalized().Dot(re));
    radian = Math::RadianToDegree(radian);

    return (radian <= angle) && (dir.Length() <= length);
}

void BossSystem::UpdateThirdAttack(TriggerSystem& aTriggerSystem)
{

    if (!myRecentlyAttacked)
    {
        if (!myThirdAttackStarted)
        {
            myCoordinator->GetComponent<ModelCollection>(myBossEntity).animator->Play("A3", false, "Idle");
            myThirdAttackStarted = true;
            SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Boss_Suck_Execute, *myPosition, true);
        }
        myThirdAttackLoadupStopwatch.Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);

        if (myThirdAttackLoadupStopwatch.IsOver())
        {
            if (!myThirdAttackVfxStarted)
            {
                auto pos = *myTransform;
                pos.GetMatrix().GetPosition().x += myLookDirection.x * 250.f;
                pos.GetMatrix().GetPosition().z += myLookDirection.y * 250.f;
                SE::CVFXManager::GetInstance().PlayVFX("BossAura", *myTransform);
                SE::CVFXManager::GetInstance().PlayVFX("BossSuck", pos);
                myThirdAttackVfxStarted = true;
                myUpdateSuck = true;
            }
            UpdateRotationAngle();
            LookAtTarget();
            MovePlayerTowardsBoss();

        }   

        if (myUpdateSuck)
        {
            myThirdStopwatch.Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);
        }

        if (myThirdStopwatch.IsOver())
        {
            Trigger myAttackTrigger;
            myAttackTrigger.event = TriggerEvent::Boss_ThirdAttack;
            myAttackTrigger.shape.asSphere.radius = myThirdAttackRadius;

            myAttackTrigger.shape.type = ShapeType::Sphere;
            myAttackTrigger.damage = myThirdAttackDamage;
            Transform transform;
            transform.SetPosition(myTransform->GetPosition());

            aTriggerSystem.CreateTrigger(myAttackTrigger, transform);
            myThirdAttackHitCount++;
            myThirdStopwatch.Reset();
        }

        if (myThirdAttackHitCount > static_cast<unsigned int>(myThirdAttackCometAmount))
        {
            myRecentlyAttacked = true;

        }
    }
    else if (myPostAttackStopwatch.IsOver())
    {
        myRecentlyAttacked = false;
        myThirdAttackStarted = false;
        myThirdAttackVfxStarted = false;
        myUpdateSuck = false;
        myThirdAttackHitCount = 0;
        myThirdAttackLoadupStopwatch.SetIsOver();
        myPostAttackStopwatch.SetIsOver();
        ToggleNewState();
    }
    
}



void BossSystem::StartSecondPhase()
{
    myUnlockedPhases++;

    mySecondPhase = true;
    SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Boss_ChangeState, *myPosition, true);
    myAudioSystem->UpdateMusicParameter("BossHealth", 65.0f);
}

void BossSystem::StartThirdPhase()
{
    myUnlockedPhases++;

    myThirdPhase = true;
    SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Boss_ChangeState, *myPosition, true);
    myAudioSystem->UpdateMusicParameter("BossHealth", 32.0f);
}

void BossSystem::MoveBoss()
{
    myMoveLocationStopwatch.Update(SE::CEngine::GetInstance()->GetDeltaTime());
    if (!myHasChosenMovePos)
    {
        myCoordinator->GetComponent<ModelCollection>(myBossEntity).animator->Play("Dive", false, "Idle");
        SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Boss_Submerge, *myPosition, true);


        if (myPosition == &myPossiblePositions[0])
        {
            if (rand() % 2 == 0)
            {
                myPosition = &myPossiblePositions[1];
            }
            else
            {
                myPosition = &myPossiblePositions[2];
            }
        }
        else if (myPosition == &myPossiblePositions[1])
        {
            if (rand() % 2 == 0)
            {
                myPosition = &myPossiblePositions[0];
            }
            else
            {
                myPosition = &myPossiblePositions[2];
            }
        }
        else if (myPosition == &myPossiblePositions[2])
        {
            if (rand() % 2 == 0)
            {
                myPosition = &myPossiblePositions[0];
            }
            else
            {
                myPosition = &myPossiblePositions[1];
            }
        }

        myHasChosenMovePos = true;
    }

    if (myMoveLocationStopwatch.GetTime() >= 3.f)
    {
        myTransform->SetPosition({ 9999.f,9999.f, 9999.f });
    }
    
    if (myMoveLocationStopwatch.IsOver())
    {
        myCoordinator->GetComponent<ModelCollection>(myBossEntity).animator->Play("Appear", false, "Idle", false);
        SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Boss_Emerge, *myPosition, true);

        myTransform->SetPosition(*myPosition);
        myHasChosenMovePos = false;
        ToggleNewState();

    }
}

void BossSystem::LookAtTarget()
{
    myTransform->Rotate({ 0, myCurrentFrameAngle * myLerpRotate, 0 });

}

void BossSystem::UpdateRotationAngle()
{
    float lerpRotate = 0.2f;
    float PI = 3.141592653589793238463f;
    Vector2f lookDirection = Vector2f(myTargetPosition->GetPosition().x, myTargetPosition->GetPosition().z) - Vector2f{ myTransform->GetPosition().x, myTransform->GetPosition().z };
    lookDirection.Normalize();
    bool negative = false;
    if (lookDirection.x < 0)
    {
        negative = true;
    }
    float LookRotateCos = lookDirection.Dot({ 0, -1 }) / (lookDirection.Length() * 1);
    float LookRotateAngleRad = acos(LookRotateCos);
    float LookRotateAngleDeg = LookRotateAngleRad * 180 / PI;

    if (negative)
    {
        LookRotateAngleDeg = 360 - LookRotateAngleDeg;
    }

    negative = false;
    myLookDirection = { -myTransform->GetTransform()(3, 1), myTransform->GetTransform()(3, 3) };

    if (myLookDirection.x < 0)
    {
        negative = true;
    }
    float enemyRotateCos = myLookDirection.Dot({ 0, 1 }) / (myLookDirection.Length() * 1);
    float enemyRotateAngleRad = acos(enemyRotateCos);
    float enemyRotateAngleDeg = enemyRotateAngleRad * 180 / PI;
    enemyRotateAngleDeg;
    if (negative)
    {
        enemyRotateAngleDeg = 360 - enemyRotateAngleDeg;
    }
    myCurrentFrameAngle = enemyRotateAngleDeg - LookRotateAngleDeg;

    if (myCurrentFrameAngle >= 180.0f)
    {
        myCurrentFrameAngle = 360 - myCurrentFrameAngle;
        myCurrentFrameAngle = -myCurrentFrameAngle;
    }
    else if (myCurrentFrameAngle < -180.0f)
    {
        myCurrentFrameAngle = 360 + myCurrentFrameAngle;
    }
    myLerpRotate = lerpRotate;
}

void BossSystem::MovePlayerTowardsBoss()
{
    myPlayerSystem->TryMoveToPosition({ myLookDirection.x * -mySpeed + myTargetPosition->GetPosition().x, myTargetPosition->GetPosition().y, myLookDirection.y * mySpeed + myTargetPosition->GetPosition().z });
    
}

void BossSystem::CheckForPhaseShift()
{
    if (myThirdPhase)
    {
        return;
    }
    if (!mySecondPhase && mySecondPhaseThreshhold > myHpComponent->GetHealth())
    {
        StartSecondPhase();
    }
    if (mySecondPhase && !myThirdPhase && myThirdPhaseThreshhold > myHpComponent->GetHealth())
    {
        StartThirdPhase();
    }
}


