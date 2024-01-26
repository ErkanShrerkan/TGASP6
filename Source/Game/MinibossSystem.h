#pragma once
#include "TriggerListener.h"
#include "Health.h"
#include "Transform.h"
#include <map>
#include <array>
#include <set>
#include <memory>
class TriggerSystem;
class Coordinator;
class Stopwatch;
class EnemySystem;
class PlayerSystem;
class AudioSystem;
enum class MinibossState
{
    Inactive,
    Active,
    Dead,
    Count
};
class MinibossSystem
{
public:
    MinibossSystem();
    ~MinibossSystem();
    void Init(Coordinator& aCoordinator, EnemySystem* aSpawner, PlayerSystem* aPlayer);
    void Spawn();

    void Update();
    void TriggerEvent();
    void SetExitPos(const Transform& aPos);
    bool AreEnemiesDead();
    Entity GetEntity();
private:
    void UpdateKubbisSpawner();
    void UpdateColorLerp();
private:

    Entity myEntity;
    Entity myBase;

    MinibossState myState;

    Vector3f myPosition;
    std::array<Vector3f, 3> KubbisPos;
    Vector4f myNewColor;
    Vector4f myOriginalColor;
    Vector4f myCurrentColor;
    Vector4f myNewAmbientColor;
    Vector4f myOriginalAmbientColor;
    Vector4f myCurrentAmbientColor;
    Transform myExit;

    std::array<Transform, 3> myKubbisSpawnLocation;
    float myAmountOfKubbisToSpawn;
    unsigned int mySpawnedAmount;

    bool myShouldSpawn = false;
    bool myShouldLerpColor = false;
    bool myEnemiesDead = false;

    Transform* myTransform;
    Transform* myBaseTransform;
    EnemySystem* myLightEnemySystem;
    PlayerSystem* myPlayerSystem;

    Coordinator* myCoordinator;
    std::shared_ptr<Stopwatch> mySpawnInterval;
    std::shared_ptr<Stopwatch> myLerpTime;
    std::set<Entity> myEnemies;

    AudioSystem* myAudioSystem;
};

