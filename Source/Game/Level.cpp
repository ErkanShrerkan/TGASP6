#include "pch.h"
#include "Level.h"

#include <Engine\DX11.h>
#include <Engine\ContentLoader.h>
#include <Engine\Engine.h>
#include <Engine\Scene.h>
#include <Engine\PointLight.h>
#include <Engine/Camera.h>
#include "Expose.h"
#include "AudioSystem.h"
#include <sstream>
#include <pix3.h>
#include "UIManager.h"
#include "StateCutscene.h"
#include "StateStack.h"
#include <Engine\VFXManager.h>
#include "ParticleSystem.h"
#include "StateMainMenu.h"

void Level::Init(Experience* anExperienceComponent, const bool& aFromLevelSelect, int anIndex)
{
    myIndex = anIndex;

    myDialogSystem.Init("Data/Dialogs.json");
    myDialogSystem.SetActiveDialog("npc$1");

    // ECS begin
    // System
    myCoordinator.RegisterSystem(myModelRenderSystem);
    //myCoordinator.RegisterSystem(myHeavyEnemySystem);
    myCoordinator.RegisterSystem(myEnemySystem);
    myCoordinator.RegisterSystem(myCollisionSystem);
    myCoordinator.RegisterSystem(myTriggerSystem);
    myCoordinator.RegisterSystem(myAnimationSystem);
    myCoordinator.RegisterSystem(myLightSystem);
    myCoordinator.RegisterSystem(myParticleSystem);
    // Components
    myCoordinator.RegisterComponentArray(myTransforms);
    myCoordinator.RegisterComponentArray(myModels);
    myCoordinator.RegisterComponentArray(myColliders);
    myCoordinator.RegisterComponentArray(myTriggers);
    myCoordinator.RegisterComponentArray(myHealths);
    myCoordinator.RegisterComponentArray(myAttackPowers);
    myCoordinator.RegisterComponentArray(myAnimators);
    myCoordinator.RegisterComponentArray(myLights);
    myCoordinator.RegisterComponentArray(myAudioComponents);
    myCoordinator.RegisterComponentArray(myParticleEmitters);

    myCoordinator.RegisterComponentArray(myEnemies);
    // ECS end

    //Player
    //myPlayerSystem.SetNavMesh(myNavMesh.myNavTriangles);
    myPlayerSystem.Init(&myCoordinator, &myTriggerSystem, &myEnemySystem, &myMinibossSystem, &myBossSystem, anExperienceComponent, anIndex);
    myExperienceComponent = anExperienceComponent;

    if (aFromLevelSelect)
    {
        myExperienceComponent->SetLevel(9);
        myPlayerSystem.SetAvailableAbilites(myFifthLevelAbilities);
    /*switch(anIndex)
    {
        case 0:
            myPlayerSystem.SetAvailableAbilites(myFirstLevelAbilities);

            break;
        case 1:

            myPlayerSystem.SetAvailableAbilites(mySecondLevelAbilities);

            break;
        case 2:

            myPlayerSystem.SetAvailableAbilites(myThirdLevelAbilities);

            break;
        case 3:

            myPlayerSystem.SetAvailableAbilites(myFourthLevelAbilities);

            break;
        case 4:

            myPlayerSystem.SetAvailableAbilites(myFifthLevelAbilities);

            break;

    }*/
    }
    else
    {
        if (anIndex == 0)
        {
            myPlayerSystem.SetAvailableAbilites(myFirstLevelAbilities);
        }
    }


    myPlayerSystem.SetNavMesh(myNavMesh.myNavTriangles);
    //

    // Enemy
    myEnemySystem.Init(&myCoordinator, &myTriggerSystem, &myPlayerSystem, anExperienceComponent);
    //

    // Heavy Enemy
    //myHeavyEnemySystem.Init(&myCoordinator, &myTriggerSystem);
    //

    //Boss
    if (myIndex == 4)
        myBossSystem.Init(myCoordinator, &myPlayerSystem);

    //Miniboss
    if (myIndex == 2)
        myMinibossSystem.Init(myCoordinator, &myEnemySystem, &myPlayerSystem);

    // Triggers begin

    //LIGHT ENEMY
    myTriggerSystem.RegisterTriggerListener(&myEnemySystem, TriggerEvent::Player_Ability_RmbAttack);
    myTriggerSystem.RegisterTriggerListener(&myEnemySystem, TriggerEvent::Player_Ability_AOE);
    myTriggerSystem.RegisterTriggerListener(&myEnemySystem, TriggerEvent::Player_Ability_Dammsugare);
    myTriggerSystem.RegisterTriggerListener(&myEnemySystem, TriggerEvent::Player_Ability_Lmb);
    myTriggerSystem.RegisterTriggerListener(&myEnemySystem, TriggerEvent::Player_Ability_Ultimate);
    myTriggerSystem.RegisterTriggerListener(&myEnemySystem, TriggerEvent::Player_Ability_Teleport);

    //HEAVY ENEMY
    /*   myTriggerSystem.RegisterTriggerListener(&myHeavyEnemySystem, TriggerEvent::Player_Ability_RmbAttack);
       myTriggerSystem.RegisterTriggerListener(&myHeavyEnemySystem, TriggerEvent::Player_Ability_AOE);
       myTriggerSystem.RegisterTriggerListener(&myHeavyEnemySystem, TriggerEvent::Player_Ability_Dammsugare);
       myTriggerSystem.RegisterTriggerListener(&myHeavyEnemySystem, TriggerEvent::Player_Ability_Lmb);
       myTriggerSystem.RegisterTriggerListener(&myHeavyEnemySystem, TriggerEvent::Player_Ability_Ultimate);*/
    //PLAYER
    myTriggerSystem.RegisterTriggerListener(&myPlayerSystem, TriggerEvent::LightEnemy_Ability_BasicAttack);
    myTriggerSystem.RegisterTriggerListener(&myPlayerSystem, TriggerEvent::Boss_FirstAttack);
    myTriggerSystem.RegisterTriggerListener(&myPlayerSystem, TriggerEvent::Boss_SecondAttack);
    myTriggerSystem.RegisterTriggerListener(&myPlayerSystem, TriggerEvent::Boss_ThirdAttack);
    myTriggerSystem.RegisterTriggerListener(&myPlayerSystem, TriggerEvent::Level_1);
    myTriggerSystem.RegisterTriggerListener(&myPlayerSystem, TriggerEvent::Level_2);
    myTriggerSystem.RegisterTriggerListener(&myPlayerSystem, TriggerEvent::Level_3);
    myTriggerSystem.RegisterTriggerListener(&myPlayerSystem, TriggerEvent::Level_4);
    myTriggerSystem.RegisterTriggerListener(&myPlayerSystem, TriggerEvent::Level_5);
    myTriggerSystem.RegisterTriggerListener(&myPlayerSystem, TriggerEvent::SpawnBoss);
    myTriggerSystem.RegisterTriggerListener(&myPlayerSystem, TriggerEvent::MinibossCutscene);
    myTriggerSystem.RegisterTriggerListener(&myPlayerSystem, TriggerEvent::CultistEnemy_Ability_Ranged);
    myTriggerSystem.RegisterTriggerListener(&myPlayerSystem, TriggerEvent::DialogCutscene);

    //BOSS
    myTriggerSystem.RegisterTriggerListener(&myBossSystem, TriggerEvent::Player_Ability_RmbAttack);
    myTriggerSystem.RegisterTriggerListener(&myBossSystem, TriggerEvent::Player_Ability_AOE);
    myTriggerSystem.RegisterTriggerListener(&myBossSystem, TriggerEvent::Player_Ability_Dammsugare);
    myTriggerSystem.RegisterTriggerListener(&myBossSystem, TriggerEvent::Player_Ability_Lmb);
    myTriggerSystem.RegisterTriggerListener(&myBossSystem, TriggerEvent::Player_Ability_Ultimate);
    myTriggerSystem.RegisterTriggerListener(&myBossSystem, TriggerEvent::SpawnBoss);
    // Triggers end

/* for (float x = 10; x < 0; ++x)
     for (float z = 10; z < 0; ++z)
     {
         Entity lightEnemy = myCoordinator.CreateEntity();
         Transform transform;
         transform.SetPosition({ x * 150.f, 0.f, z * 150.f });

         Collider collider;
         collider.shape.asSphere.radius = 30.0f;
         myCoordinator.AddComponent(lightEnemy, collider);

         myCoordinator.AddComponent(lightEnemy, SE::DX11::Content->GetModelFactory().GetModel("Models/CH_EY_Kubb/CH_EY_Kubb.erc"));
         myCoordinator.AddComponent(lightEnemy, transform);
         myCoordinator.AddComponent(lightEnemy, KubbLeffe({ myPlayer }));
     }*/

    /* Transform enemPos;
     enemPos.SetPosition({ 6118.161f, 568.332f, -1189.257f });
     myEnemySystem.SpawnEnemy(enemPos, myPlayer, Enemy::eType::eCultist, 800.f);*/

     /*/Trigger trigger;
     trigger.shape.type = ShapeType::Sphere;
     trigger.shape.asSphere.radius = 150.f;
     trigger.event = TriggerEvent::Player_Ability_BasicAttack;
     trigger.lifeTime = -1.0f;
     Transform triggerPosition;
     triggerPosition.SetPosition({500.f, 0.f, 500.f});
     myTriggerSystem.CreateTrigger(trigger, triggerPosition);*/



     //myCutsceneCamera.Init
     ////craschar f1menyn
     //EXPOSE(myListenerDistance);
     //EXPOSE(myListenerOffset);

    myAudiosystem = AudioSystem::GetInstance();
}

void Level::Update()
{
    ENGINE->GetActiveScene()->GetMainCamera()->Update(SE::CEngine::GetInstance()->GetDeltaTime());
#ifdef _DEBUG
    PIXBeginEvent(PIX_COLOR_INDEX(2), __FUNCTION__);
    PIXBeginEvent(PIX_COLOR_INDEX(3), "TriggerSystem::DeleteDeadTriggers");
#endif // DEBUG
    myTriggerSystem.DeleteDeadTriggers();
#ifdef _DEBUG
    PIXEndEvent();
    PIXBeginEvent(PIX_COLOR_INDEX(3), "CollisionSystem::CheckCollisions");
#endif // DEBUG
    myCollisionSystem.CheckCollisions(myTriggerSystem);
#ifdef _DEBUG
    PIXEndEvent();
    PIXBeginEvent(PIX_COLOR_INDEX(3), "TriggerSystem::DistributeEvents");
#endif // DEBUG
    myTriggerSystem.DistributeEvents();
#ifdef _DEBUG
    PIXEndEvent();
    PIXBeginEvent(PIX_COLOR_INDEX(3), "LightEnemySystem::Update");
#endif // DEBUG
    if ((&Singleton<UIManager>().GetStateStack().GetCurrentState() == Singleton<UIManager>().GetStateGameplay()) || Singleton<UIManager>().GetStateStack().GetCurrentState().GetStateID() == eStateID::Cutscene && GetFocus())
    {
        myEnemySystem.Update();
    }
#ifdef _DEBUG
    PIXEndEvent();
    PIXBeginEvent(PIX_COLOR_INDEX(3), "PlayerSystem::Update");
#endif // DEBUG

    //printf(GetFocus() ? "True \n" : "False \n");
    if((& Singleton<UIManager>().GetStateStack().GetCurrentState() == Singleton<UIManager>().GetStateGameplay()) && GetFocus())
    {
        myPlayerSystem.Update();
        Singleton<UIManager>().GetStateGameplay()->RenderXPBar(myExperienceComponent->GetXPBarPosition());
        Singleton<UIManager>().GetStateGameplay()->SetCurrentLevel(myExperienceComponent->GetLevel());
    }


#ifdef _DEBUG
    PIXEndEvent();
#endif // DEBUG
    if (myIndex == 4)
    {
#ifdef _DEBUG
        PIXBeginEvent(PIX_COLOR_INDEX(3), "BossSystem::Update");
#endif // DEBUG

        myBossSystem.Update(myTriggerSystem);
        
#ifdef _DEBUG
        PIXEndEvent();
#endif // DEBUG
    }
   
    if (myIndex == 2)
    {
#ifdef _DEBUG
        PIXBeginEvent(PIX_COLOR_INDEX(3), "MinibossSystem::Update");
#endif // DEBUG

        myMinibossSystem.Update();

#ifdef _DEBUG
        PIXEndEvent();
#endif // DEBUG
    }

#ifdef _DEBUG
    PIXBeginEvent(PIX_COLOR_INDEX(3), "ModelRenderSystem::Render");
#endif // DEBUG
    myModelRenderSystem.Render();
#ifdef _DEBUG
    PIXEndEvent();
    PIXBeginEvent(PIX_COLOR_INDEX(3), "AnimationSystem::Update");
#endif // DEBUG
	myAnimationSystem.Update(SE::CEngine::GetInstance()->GetDeltaTime());
#ifdef _DEBUG
    PIXEndEvent();
#endif // DEBUG

#ifdef _DEBUG
    PIXBeginEvent(PIX_COLOR_INDEX(3), "SE::CAudioEngine::Update");
#endif // DEBUG
    Transform transform;
    transform.SetTransform(SE::CEngine::GetInstance()->GetActiveScene()->GetMainCamera()->GetTransform());
    Vector3f cameraPos = SE::CEngine::GetInstance()->GetActiveScene()->GetMainCamera()->GetPosition() + SE::CEngine::GetInstance()->GetActiveScene()->GetMainCamera()->GetRenderOffset();
    Vector3f playerPos = myPlayerSystem.GetCurrentPosition();
    Vector3f dif = playerPos - cameraPos;
    dif.y = 0;
    myListenerDistance = 0.4f;
    myListenerOffset = 300.0f;
    transform.SetPosition(transform.GetPosition()*myListenerDistance + SE::CEngine::GetInstance()->GetActiveScene()->GetMainCamera()->GetRenderOffset() + dif.GetNormalized() * myListenerOffset);
    myAudiosystem->Update(transform, true);

#ifdef _DEBUG
    PIXEndEvent();
    PIXBeginEvent(PIX_COLOR_INDEX(3), "Level::RenderDebugNavMesh");
#endif // DEBUG

    RenderDebugNavMesh();

#ifdef _DEBUG
    PIXEndEvent();
    PIXBeginEvent(PIX_COLOR_INDEX(3), "DialogSystem::Update");
#endif // DEBUG

    myDialogSystem.Update();

#ifdef _DEBUG
    PIXEndEvent();
#endif // DEBUG

#ifdef _DEBUG
    PIXBeginEvent(PIX_COLOR_INDEX(3), "LightSystem::Render");
#endif // DEBUG
    myLightSystem.Render();
#ifdef _DEBUG
    PIXEndEvent();
    PIXBeginEvent(PIX_COLOR_INDEX(3), "ParticleSystem::Update");
#endif // DEBUG
    myParticleSystem.Update();
#ifdef _DEBUG
    PIXEndEvent();
    PIXBeginEvent(PIX_COLOR_INDEX(3), "ParticleSystem::Render");
#endif // DEBUG
    myParticleSystem.Render();
#ifdef _DEBUG
    PIXEndEvent();
    PIXEndEvent();
#endif // DEBUG
}

void Level::RenderDebugNavMesh()
{
    /*if (GetAsyncKeyState('Y'))
    {
        myShouldRenderPathfindDebug = true;
    }
    if (GetAsyncKeyState('U'))
    {
        myShouldRenderPathfindDebug = false;
    }*/
    if (myShouldRenderPathfindDebug)
    {
        for (auto& instance : myPathfindDebugInstances)
        {
            instance->Render();

        }
        //myNavMeshModel->Render();
        myDebugClickPos.Render();
    }
}



#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <fstream>
#include <Engine/Animator.h>
bool Level::PopulateFromJson(const std::string& aFilePath)
{
    Singleton<UIManager>().GetStateStack().GetCutscene()->Reset();
    // Load ENTIRE file into RAM before parsing
    std::string level = "Levels/";
    level += aFilePath;
    std::ifstream file{ std::string(level) };
    std::string content{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
    file.close();
    

    rapidjson::StringStream stream(content.c_str());
    rapidjson::Document document;
    if (document.ParseStream(stream).HasParseError())
    {
        perr("<%s> Parse error at offset %u: \"%s\", when loading file \"%s\"",
            "Level::PopulateFromJson",
            (unsigned)document.GetErrorOffset(),
            rapidjson::GetParseError_En(document.GetParseError()),
            aFilePath.data()
        );
        return false;
    }

    auto jsonObjects = document["GameObjects"].GetArray();

    for (rapidjson::Value& jsonObject : jsonObjects)
    {
        if (jsonObject.HasMember("DummyObject"))
            continue;

        std::string tag = jsonObject["Tag"].GetString();

        // Remove trailing whitespace
        tag.erase(std::find_if(tag.rbegin(), tag.rend(), [](unsigned char c)
            {
                return !std::isspace(c);
            }).base(), tag.end());

        std::string name = jsonObject["Name"].GetString();
        Vector3f position(
            jsonObject["Position"]["x"].GetFloat(),
            jsonObject["Position"]["y"].GetFloat(),
            jsonObject["Position"]["z"].GetFloat()
        );
        Vector3f rotation(
            jsonObject["Rotation"]["x"].GetFloat() * - 1.0f,
            jsonObject["Rotation"]["y"].GetFloat() + 180.f,
            jsonObject["Rotation"]["z"].GetFloat() * - 1.0f
        );
        Vector3f scale(
            jsonObject["Scale"]["x"].GetFloat(),
            jsonObject["Scale"]["y"].GetFloat(),
            jsonObject["Scale"]["z"].GetFloat()
        );
        
        Transform transform;
        transform.SetPosition(position * 100.f);
        transform.SetRotation(rotation);
        transform.SetScale(scale);

        if (tag == "Untagged")
        {
            Entity entity = myCoordinator.CreateEntity();

            std::string modelPath = "Models/" + name + "/" + name + ".erc";
            SE::CModel* model = SE::DX11::Content->GetModelFactory().GetModel(modelPath);
  

            if (name == "CH_NPC_Cultist")
            {
               
                myCoordinator.AddComponent(entity, ModelCollection{ model, new SE::CAnimator(model) });
                myCoordinator.GetComponent<ModelCollection>(entity).animator->AddAnimation("Models/CH_NPC_Cultist/CH_NPC_Cultist_Idle_AN.myr", "Idle");
                myCoordinator.GetComponent<ModelCollection>(entity).animator->AddAnimation("Models/CH_NPC_Cultist/CH_NPC_Cultist_EndJump_AN.myr", "Jump");
                myCoordinator.GetComponent<ModelCollection>(entity).animator->Play("Idle", true);
                myCoordinator.GetComponent<ModelCollection>(entity).animator->SetFallbackAnimation("Idle");
                transform.SetScale({ 1.f, 1.f, 1.f });

                if (myIndex == 0)
                {
                    Trigger trigger;
                    trigger.event = TriggerEvent::DialogCutscene;

                    trigger.lifeTime = FLT_MAX;
                    trigger.shape.type = ShapeType::Sphere;
                    trigger.shape.asSphere.radius = 300.f;

                    Transform t = transform;

                    myTriggerSystem.CreateTrigger(trigger, t);
                }
                if (myIndex == 4)
                {
                    
                    myBossSystem.SetNpcReference(entity);
                }
            }
            else
            {
                myCoordinator.AddComponent(entity, ModelCollection{ model });
            }
            myCoordinator.AddComponent(entity, transform);

            
        }
        else if (tag == "PointLight")
        {
            std::string lightData = jsonObject["CustomData"].GetString();
            rapidjson::StringStream lightStream(lightData.c_str());
            rapidjson::Document lightDocument;
            lightDocument.ParseStream(lightStream);

            float lightRange = lightDocument["Range"].GetFloat();
            float lightIntensity = lightDocument["Intensity"].GetFloat();
            auto& jsonColor = lightDocument["Color"];
            Vector3f lightColor;
            lightColor.r = jsonColor["r"].GetFloat();
            lightColor.g = jsonColor["g"].GetFloat();
            lightColor.b = jsonColor["b"].GetFloat();

            BaseLight* pointLight = new BaseLight();
            if (name == "Flicker")
            {
#if USE_HDR
                lightRange = 750.f;
                lightIntensity = 5.f;
#endif
                pointLight->SetAsFlicker();
            }
#if USE_HDR
            else
            {
                lightRange *= 40.0f;
                lightIntensity *= 5.f;
            }
#else
                lightRange *= 100.0f;
                lightIntensity *= 10.f;
#endif
            pointLight->SetColor(lightColor);
            pointLight->SetIntensity(lightIntensity /* *10.0f*/);
            pointLight->SetRange(lightRange /** 100.0f*/);
            pointLight->SetPosition(position * 100.0f);

            Entity entity = myCoordinator.CreateEntity();
            myCoordinator.AddComponent(entity, transform);
            myCoordinator.AddComponent(entity, pointLight);

            continue;
        }
        else if (tag == "LightEnemy")
        {
            myEnemySystem.SpawnEnemy(transform, myPlayer,Enemy::eType::eKubbLeffe, 800.0f);
            continue;
        }
        else if(tag == "MediumEnemy")
        {
            myEnemySystem.SpawnEnemy(transform, myPlayer, Enemy::eType::eBogSchyte, 800.0f);
            continue;
        }
        else if (tag == "Cultist")
        {
            myEnemySystem.SpawnEnemy(transform, myPlayer, Enemy::eType::eCultist, 800.0f);
            continue;
        }
        else if(tag == "Champion")
        {
            myEnemySystem.SpawnEnemy(transform, myPlayer, Enemy::eType::eChampion, 800.0f);
        }
        else if (tag == "Player")
        {
            myCoordinator.GetComponent<Transform>(myPlayer).SetPosition(position*100.0f);
            myPlayerSystem.SetPlayerSpawn(position * 100.f);
            auto* mm = reinterpret_cast<StateMainMenu*>(&Singleton<UIManager>().GetStateStack().GetStateFromId(eStateID::MainMenu));
            mm->SetCamPos(position * 100.f, myIndex);
        }
        else if (tag == "LevelSwitch")
        {
            std::stringstream ss;
            ss << name;

            int index;
            ss >> index;
            Trigger trigger;
            switch (index)
            {
            case 1:
                trigger.event = TriggerEvent::Level_1;
                trigger.damage = 1;
                break;
            case 2:
                trigger.event = TriggerEvent::Level_2;
                trigger.damage = 2;
                break;
            case 3:
                trigger.event = TriggerEvent::Level_3;
                break;
            case 4:
                trigger.event = TriggerEvent::Level_4;
                trigger.damage = 4;
                myMinibossSystem.SetExitPos(transform);

                break;
            case 5:
                trigger.event = TriggerEvent::Level_5;
                trigger.damage = 5;
                break;

            default:
                printji("Error reading correct level switch");
                break;
            }
            trigger.lifeTime = FLT_MAX;
            trigger.shape.type = ShapeType::Sphere;
            trigger.shape.asSphere.radius = 300.f;



            myTriggerSystem.CreateTrigger(trigger, transform);
        }
        else if (tag == "CameraKeyframe")
        {
            auto* cs = reinterpret_cast<StateCutscene*>(&Singleton<UIManager>().GetStateStack().GetStateFromId(eStateID::Cutscene));
            int enumVal = std::stoi(name);
            cs->AddCutsceneKeyframe(static_cast<eCutscenes>(enumVal), transform);      
        }
        else if (tag == "CutsceneTrigger")
        {
            Trigger trigger;

            trigger.lifeTime = FLT_MAX;
            trigger.shape.type = ShapeType::Sphere;
            trigger.shape.asSphere.radius = 600.f;
            trigger.event = TriggerEvent::MinibossCutscene;

            myTriggerSystem.CreateTrigger(trigger, transform);
        }
        else if (tag == "VFX")
        {
        if (name == "Portal")
        {
            int i = 0;
            i;
        }
            SE::CVFXManager::GetInstance().PlayVFX(name, transform);
        }
        else if (tag == "Particle")
        {
            Entity entity = myCoordinator.CreateEntity();

            SE::CParticleEmitter* p = SE::CEngine::GetInstance()->GetContentLoader()->GetParticleFactory().GetParticleEmitter(name);
	        ParticleEmitter en;
            myCoordinator.AddComponent(entity, transform);
	        myCoordinator.AddComponent<ParticleEmitter>(entity, en);
	        myCoordinator.GetComponent<ParticleEmitter>(entity).Init(p);
        }
    }


    std::string navMeshPath = level.substr(0, level.size() - 4);
    navMeshPath += "_Navmesh.obj"; // TODO change when navmeshes are updated

    myNavMesh.myNavTriangles = SE::DX11::Content->GetNavMeshLoader().LoadNavMesh(navMeshPath)->myNavTriangles;
    std::string heightPath = navMeshPath.substr(0, navMeshPath.length() - 4);
    heightPath += "16.obj";
    if (navMeshPath == "Levels/AlexLevels - Lvl2_Navmesh.obj")
    {
        //myHeightMesh.myNavTriangles = SE::DX11::Content->GetNavMeshLoader().LoadNavMesh(heightPath)->myNavTriangles;
    }

    myPlayerSystem.SetNavMesh(myNavMesh.myNavTriangles);
    //myPlayerSystem.SetHeightMesh(myHeightMesh.myNavTriangles);
    myPlayerSystem.FindCurrentTriangle(myPlayerSystem.GetCurrentPosition());
    myPlayerSystem.CalculatePlayerYPosition();


    myPlayerSystem.SetDebugClickPos(&myDebugClickPos);
    myEnemySystem.SetNavMesh(myNavMesh.myNavTriangles);

    //Boss cutscene trigger
    if (myIndex == 4)
    {
        Trigger trigger;

        trigger.lifeTime = FLT_MAX;
        trigger.shape.type = ShapeType::Sphere;
        trigger.shape.asSphere.radius = 600.f;
        trigger.event = TriggerEvent::SpawnBoss;

        Transform transform;
        transform.SetPosition({ 5000.f, 546.f, -1700.f });

        myTriggerSystem.CreateTrigger(trigger, transform);
    }

#pragma region CleaUpLater


    //SE::CModel* model2 = SE::DX11::Content->Load<SE::CModel>("Models/CH_EY_Kubb/CH_EY_Kubb.fbx");
   /* for (int i = 0; i < myNavMesh.myNavVertices.size(); i++)
        {
        	SE::CModelInstance* modelinstance = new SE::CModelInstance();
            modelinstance->Init(model2);
        	SE::SNavVertex navVertex = myNavMesh.myNavVertices[i];
            modelinstance->SetPosition({ navVertex.myPosition.x,navVertex.myPosition.y,navVertex.myPosition.z });
            modelinstance->SetScale({ 0.5f,0.5f,0.5f });
        	myPathfindDebugInstances.push_back(modelinstance);
        }

        for (auto& navTriangle : myNavMesh.myNavTriangles)
        {
            SE::CModelInstance* modelinstance = new SE::CModelInstance();
            modelinstance->Init(model2);
        	Vector3f pos = navTriangle->myCentroid;
            modelinstance->SetPosition({ pos.x,(pos.y),pos.z});
            modelinstance->SetScale({ 0.3f,0.3f,0.3f });
        	myPathfindDebugInstances.push_back(modelinstance);
        }

        SE::CModel* model = SE::DX11::Content->GetModelFactory().LoadNavMeshModel(navMeshPath.c_str());
        myNavMeshModel = new SE::CModelInstance();
        myNavMeshModel->Init(model);
        myNavMeshModel->SetScale({ 100,100,100 });
        myNavMeshModel->SetPosition({ 0,0,0 });*/

        myDebugClickPos = SE::CModelInstance();
        //myDebugClickPos->Init(model2);
        myDebugClickPos.SetPosition({ -0, 0.0f, -0 });
        myDebugClickPos.SetScale({ 0.5f,0.6f,0.5f });
        
        myPlayerSystem.SetDebugClickPos(&myDebugClickPos);

#pragma endregion

    return true;
}

PlayerSystem& Level::GetPlayerSystem()
{
    return myPlayerSystem;
}

BossSystem& Level::GetBossSystem()
{
    return myBossSystem;
}

Coordinator& Level::GetCoordinator()
{
    return myCoordinator;
}
