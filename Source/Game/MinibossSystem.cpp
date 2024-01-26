#include "pch.h"
#include "MinibossSystem.h"
#include "Coordinator.h"
#include "Collider.h"
#include "Engine\DX11.h"
#include "Engine\ContentLoader.h"
#include <Engine\Input.h>
#include "TriggerSystem.h"
#include <Engine\Engine.h>
#include "ModelCollection.h"
#include "Expose.h"
#include "EnemySystem.h"
#include "UIManager.h"
#include "StateStack.h"
#include "StateCutscene.h"
#include "Engine/Animator.h"
#include "PlayerSystem.h"
#include "GameWorld.h"
#include "LevelHandler.h"
#include "Engine/AudioEngine.h"
#include "AudioSystem.h"
#include "Postmaster.h"
#include <Engine/VFXManager.h>
MinibossSystem::MinibossSystem()
{
    myTransform = nullptr;
    myLightEnemySystem = nullptr;
    myCoordinator = nullptr;
    myState = MinibossState::Count;
    myAmountOfKubbisToSpawn = 0;
}

MinibossSystem::~MinibossSystem()
{
    UNEXPOSE(myPosition);
    UNEXPOSE(myAmountOfKubbisToSpawn);
    UNEXPOSE(KubbisPos[0]);
    UNEXPOSE(KubbisPos[1]);
    UNEXPOSE(KubbisPos[2]);
}

void MinibossSystem::Init(Coordinator& aCoordinator, EnemySystem* aSpawner, PlayerSystem* aPlayer)
{
    EXPOSE(myPosition);
    EXPOSE(myAmountOfKubbisToSpawn);
    EXPOSE(KubbisPos[0]);
    EXPOSE(KubbisPos[1]);
    EXPOSE(KubbisPos[2]);
    //EXPOSE(myColorTransition);

    myNewColor = {0.011f, 0.277f, 0.387f, 1.f};
    myNewAmbientColor = {0.27f, 0.026f, 0.026f, 1.f};

    mySpawnInterval = std::make_shared<Stopwatch>(0.9f);
    myLerpTime = std::make_shared<Stopwatch>(3.f);

    myBase = aCoordinator.CreateEntity();
    myEntity = aCoordinator.CreateEntity();
    myCoordinator = &aCoordinator;
    myLightEnemySystem = aSpawner;
    myPlayerSystem = aPlayer;
    Collider collider;
    collider.shape.asSphere.radius = 500.f;
    collider.shape.type = ShapeType::Sphere;

    aCoordinator.AddComponent(myEntity, collider);

    Transform transform;
    aCoordinator.AddComponent(myEntity, transform);
    myTransform = &aCoordinator.GetComponent<Transform>(myEntity);
    Transform btransform;
    aCoordinator.AddComponent(myBase, btransform);
    myBaseTransform = &aCoordinator.GetComponent<Transform>(myBase);

    SE::CModel* cmodel = SE::DX11::Content->GetModelFactory().GetModel("Models/EN_P_Shittingman/EN_P_Shittingman.erc");
    ModelCollection model{ cmodel };



    SE::CModel* cbase = SE::DX11::Content->GetModelFactory().GetModel("Models/EN_P_Shittingman/EN_P_ShittingmanStaticBase.erc");
    ModelCollection base{ cbase };

    aCoordinator.AddComponent(myEntity, model);
    aCoordinator.AddComponent(myBase, base);

    myAudioSystem = AudioSystem::GetInstance();
}

void MinibossSystem::Spawn()
{
    myState = MinibossState::Inactive;
    myPosition = { 2600.f, 1000.f, 150.f};

    myTransform->SetPosition(myPosition);
    myTransform->SetScale({ 200.f, 200.f, 200.f });
    myTransform->Rotate({ 0.f, 90.f, 0.f });

    myBaseTransform->SetPosition(myPosition);
    myBaseTransform->SetScale({ 200.f, 200.f, 200.f });
    myBaseTransform->Rotate({ 0.f, 90.f, 0.f });

    myCoordinator->GetComponent<Transform>(myBase).SetPosition(myPosition);
    myCoordinator->GetComponent<Transform>(myBase).SetScale({ 200.f, 200.f, 200.f });
    myCoordinator->GetComponent<Transform>(myBase).Rotate({ 0.f, 90.f, 0.f });
}

void MinibossSystem::Update()
{
    myKubbisSpawnLocation[0].SetPosition(KubbisPos[0]);
    myKubbisSpawnLocation[1].SetPosition(KubbisPos[1]);
    myKubbisSpawnLocation[2].SetPosition(KubbisPos[2]);
    myTransform->SetPosition(myPosition);

    if (myState == MinibossState::Count)
    {
        Spawn();
    }
    if (myShouldSpawn)
    {
        mySpawnInterval->Update(SE::CEngine::GetInstance()->GetDeltaTime());
        UpdateKubbisSpawner();
    }
    if (myShouldLerpColor)
    {
        myLerpTime->Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);
        myCurrentColor = Math::Lerp(myOriginalColor, myNewColor, myLerpTime->GetTime() / myLerpTime->GetTimeInterval());
        myCurrentAmbientColor = Math::Lerp(myOriginalAmbientColor, myNewAmbientColor, myLerpTime->GetTime() / myLerpTime->GetTimeInterval());
        GameWorld::GetInstance()->GetLevelHandler().GetActiveLevelSettings().lightColor = myCurrentColor;
        GameWorld::GetInstance()->GetLevelHandler().GetActiveLevelSettings().ambientColor = myCurrentAmbientColor;
        if (myLerpTime->IsOver())
        {
            myShouldLerpColor = false;
        }
    }
    if (myState == MinibossState::Dead)
    {
        std::set<Entity> dead;
        for (auto& enemy : myEnemies)
        {
            if (myCoordinator->GetComponent<Health>(enemy).IsEntityDead())
            {
                dead.insert(enemy);
            }
        }
        for (auto& enemy : dead)
        {
            myEnemies.erase(enemy);
        }
        if (0 == myEnemies.size())
        {
            if (!myEnemiesDead)
            {
                myEnemiesDead = true;
                Singleton<UIManager>().GetStateGameplay()->SetObjectiveIndex(6);
                Postmaster::GetInstance()->SendMail(eMessage::eStartCutscene);
                Singleton<UIManager>().GetStateStack().GetCutscene()->PlayCutscene(eCutscenes::MinibossPortalOpen);
                
                SE::CVFXManager::GetInstance().PlayVFX("Portal", myExit);
            }
            
        }
    }
}

Entity MinibossSystem::GetEntity()
{
    return myEntity;
}


void MinibossSystem::TriggerEvent()
{
    if (myState == MinibossState::Inactive)
    {
        Singleton<UIManager>().GetStateGameplay()->SetObjectiveIndex(5);

        SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::MiniBoss_Explode);
        myAudioSystem->UpdateMusicParameter("MiniBossHealth", 1.0f);

        myState = MinibossState::Active;
        myShouldSpawn = true;
        Singleton<UIManager>().GetStateStack().GetCutscene()->PlayCutscene(eCutscenes::MinibossEnd);
        Postmaster::GetInstance()->SendMail(eMessage::eStartCutscene);

        myPlayerSystem->SetTryToPlayUltimateVFX(true);
        myPlayerSystem->SetAbilityStatus(PlayerSystem::Abilities::ULTIMATE, true);

        SE::CModel* cmodel = SE::DX11::Content->GetModelFactory().GetModel("Models/EN_P_Shittingman/CH_EY_Shittingman.erc");

        myCoordinator->GetComponent<ModelCollection>(myEntity).model = cmodel;

        myCoordinator->GetComponent<ModelCollection>(myEntity).animator = new SE::CAnimator(cmodel);

        myCoordinator->GetComponent<ModelCollection>(myEntity).animator->AddAnimation("Models/EN_P_Shittingman/CH_EY_Shittingman_Shattering_AN.myr", "Shatter");

        myTransform->SetScale({ 1.f, 1.f, 1.f });

        myCoordinator->GetComponent<ModelCollection>(myEntity).animator->Play("Shatter", false);

  		myShouldLerpColor = true;
        myOriginalColor = GameWorld::GetInstance()->GetLevelHandler().GetActiveLevelSettings().lightColor;
        myOriginalAmbientColor = GameWorld::GetInstance()->GetLevelHandler().GetActiveLevelSettings().ambientColor;
        myCurrentColor = myOriginalColor;
        myCurrentAmbientColor = myOriginalAmbientColor;
    }
    //Play animation for breaking statue
    //Turn world red, maybe send event with postmaster
    //Spawn kubbisar
}

void MinibossSystem::SetExitPos(const Transform& aPos)
{
    myExit = aPos;
}

bool MinibossSystem::AreEnemiesDead()
{
    return myEnemiesDead;
}

void MinibossSystem::UpdateKubbisSpawner()
{
    if (mySpawnInterval->IsOver() && myShouldSpawn)
    {
        myKubbisSpawnLocation[0].SetPosition({ myKubbisSpawnLocation[0].GetPosition().x,myKubbisSpawnLocation[0].GetPosition().y,myKubbisSpawnLocation[0].GetPosition().z});
        myEnemies.insert(myLightEnemySystem->SpawnEnemy(myKubbisSpawnLocation[0], 0, Enemy::eType::eKubbLeffe, 10000.0f));
        myKubbisSpawnLocation[1].SetPosition({ myKubbisSpawnLocation[1].GetPosition().x ,myKubbisSpawnLocation[1].GetPosition().y,myKubbisSpawnLocation[1].GetPosition().z});
        myEnemies.insert(myLightEnemySystem->SpawnEnemy(myKubbisSpawnLocation[1], 0, Enemy::eType::eKubbLeffe, 10000.0f));

        myKubbisSpawnLocation[2].SetPosition({ myKubbisSpawnLocation[2].GetPosition().x,myKubbisSpawnLocation[2].GetPosition().y,myKubbisSpawnLocation[2].GetPosition().z});
        myEnemies.insert(myLightEnemySystem->SpawnEnemy(myKubbisSpawnLocation[2], 0, Enemy::eType::eKubbLeffe, 10000.0f));
        mySpawnedAmount++;
        if (mySpawnedAmount >= myAmountOfKubbisToSpawn)
        {
            myShouldSpawn = false;
            myState = MinibossState::Dead;
        }
    }
}

void MinibossSystem::UpdateColorLerp()
{
}
