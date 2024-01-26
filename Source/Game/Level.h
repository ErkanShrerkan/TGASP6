#pragma once

// ECS
#include "Coordinator.h"
#include "ModelRenderSystem.h"
#include "EnemySystem.h"
#include "CollisionSystem.h"
#include "TriggerSystem.h"
#include "PlayerSystem.h"
#include "BossSystem.h"
#include "MinibossSystem.h"
#include "NavMesh.h"
#include "AnimatorSystem.h"
#include "DialogSystem.h"
#include "LightSystem.h"
#include "ParticleSystem.h"
#include <Engine/Camera.h>
#include <array>
#include "Experience.h"
class Level
{
private:
    int myIndex;
public:
    bool operator<(const Level& aLevel)
    {
        return myIndex < aLevel.myIndex;
    }
    inline const int GetIndex() const noexcept
    {
        return myIndex;
    }

public:
    void Init(Experience* anExperienceComponent, const bool& aFromLevelSelect, int anIndex = 0);
    void Update();
    void RenderDebugNavMesh();
    bool PopulateFromJson(const std::string& aFilePath);
    PlayerSystem& GetPlayerSystem();
    BossSystem& GetBossSystem();
    Coordinator& GetCoordinator();
private:
    // Coordinator
    Coordinator myCoordinator;
    // Systems
    ModelRenderSystem myModelRenderSystem;
    EnemySystem myEnemySystem;
    CollisionSystem myCollisionSystem;
    TriggerSystem myTriggerSystem;
    PlayerSystem myPlayerSystem;
    BossSystem myBossSystem;
    MinibossSystem myMinibossSystem;
    AnimatorSystem myAnimationSystem;
    DialogSystem myDialogSystem;
    LightSystem myLightSystem;
    ParticleSystem myParticleSystem;
    AudioSystem* myAudiosystem;
    float myListenerDistance;
    float myListenerOffset;
    // Components
    ComponentArray<SE::CTransform> myTransforms;
    ComponentArray<ModelCollection> myModels;
    ComponentArray<Enemy> myEnemies; // <- not final
    ComponentArray<Collider> myColliders;
    ComponentArray<Trigger> myTriggers;
    ComponentArray<Health> myHealths;
    ComponentArray<AttackPower> myAttackPowers;
    ComponentArray<SE::CAnimator*> myAnimators;
    ComponentArray<Light> myLights;
    ComponentArray<AudioComponent> myAudioComponents;
    ComponentArray<ParticleEmitter> myParticleEmitters;

    //NavMesh
    NavMesh myNavMesh;
    NavMesh myHeightMesh;
    std::vector<SE::CModelInstance*> myPathfindDebugInstances;
    bool myShouldRenderPathfindDebug = false;
    SE::CModelInstance* myNavMeshModel;
    SE::CModelInstance* myDebugClickPos;

    //Camera for cutscenes
    SE::CCamera myCutsceneCamera;
    //Player
    Entity myPlayer;

    Experience* myExperienceComponent;

    std::array<bool, 7> myFirstLevelAbilities = {false, false, false, false, true, true, true};
    std::array<bool, 7> mySecondLevelAbilities = { true, false, false, false, true, true, true };
    std::array<bool, 7> myThirdLevelAbilities = { true, true, false, false, true, true, true };
    std::array<bool, 7> myFourthLevelAbilities = { true, true, true, true, true, true, true };
    std::array<bool, 7> myFifthLevelAbilities = { true, true, true, true, true, true, true };
};
