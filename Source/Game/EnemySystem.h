#pragma once
#include "System.h"
#include "Enemy.h"
#include "Transform.h"
#include "TriggerListener.h"
#include "Coordinator.h"
#include "Health.h"
#include "TriggerSystem.h"
#include "Audio.h"
#include "PlayerSystem.h"
#include "Engine/PathFinding.h"
#include "Observer.h"

#include <thread>
#include <atomic>

#include "Experience.h"

namespace SE
{
    struct SNavTriangle;
    struct SNavMesh;
}

class EnemySystem
    : public System<Enemy, Transform, Health,AudioComponent>
    , public TriggerListener, public Observer
{

    friend class PlayerSystem;

public:
    ~EnemySystem();
    void Init(Coordinator* aCoordinator, TriggerSystem* aTriggerSystem, PlayerSystem* aPlayerSystem, Experience* anExperienceComponent);
    void Update();
    void PlayerGainXP(Enemy& leffe);
    void MoveEnemy(Transform& transform, Vector3f& position, Enemy& leffe, Vector3f& posDif, const Entity& enemy, Enemy::eType anEnemyType);
    bool TryMoveToPosition(const Vector3f& aPosToMoveTo);
    void SetNavMesh(std::vector<SE::SNavTriangle*> someNavTriangles);
    void SnapYPositionToNavMesh(const Entity& anEntity);
    double CalcTriangleArea(double x1, double y1, double x2, double y2, double x3, double y3);
    bool PointIsInsideTriangle(double x, double y, double x1, double y1, double x2, double y2, double x3, double y3);
    void OnTriggerEnter(const TriggerData& someTriggerData) override;
    void BasicAttack(Transform& aTransform, Enemy::eType anEnemyType);
    void CultistAttack(Transform aTransform);
    void CultistAnticipation(Transform aTransform);
    void BoidBehaviour();
    void StopWalking();

    void RecieveMessage(eMessage aMsg);

    Entity SpawnEnemy(Transform& aTransform, Entity aPlayer, Enemy::eType anEnemyType, const float& aAggroRange);


    void PathFind(const Entity& anEntity);

private:
    void RotateEnemyTowards(Transform& anEnemyTransform, const Vector3f& anEnemyPosition, const Vector3f& aTarget);
    SE::SNavTriangle* FindCurrentTriangle(const Vector3f& aPosition, bool& aTriangleWasHit);
    void Thread();


    float myHealth = 100;
    Coordinator* myCoordinator;
    TriggerSystem* myTriggersystem;
    AudioComponent myWalkAudio;

    SE::SNavTriangle* myPlayerNavTriangle;
    std::vector<SE::SNavTriangle*> myNavTriangles;
    PlayerSystem* myPlayerSystem;

    std::map<SE::SNavTriangle*, SE::CPathFinding::SNewNode> myPreviousFloodFillNodes;
    std::map<SE::SNavTriangle*, SE::CPathFinding::SNewNode> myNextFloodFillNodes;
    bool myIsPathFilling = false;
    bool myIsWaitingForPathfindThread = false;
    bool myIsPathfinding = false;
    int myFrameCount = 0;

    std::thread myPathFindThread;
    bool mySystemIsRunning = true;

    //DAMAGE

    std::array<float, 4> myEnemyDamage;

    float myBogScytheDamage;
    float myKubbLeffeDamage;
    float myCultistDamage;
    float myChampionDamage;

    float myCritChance;
    float myCritMultiplier;

    //MOVEMENT SPEED

    std::array<float, 4> myEnemyMovementSpeed;

    float myBogScytheMovementSpeed;
    float myKubbLeffeMovementSpeed;
    float myCultistMovementSpeed;
    float myChampionMovementSpeed;

    std::array<float, 4> myEnemyStopDistance;

    float myBogScytheStopDistance;
    float myKubbLeffeStopDistance;
    float myCultistStopDistance;
    float myChampionStopDistance;

    std::array<float, 4> myEnemyAttackCooldown;

    float myBogScytheAttackCooldown;
    float myKubbLeffeAttackCooldown;
    float myCultistAttackCooldown;
    float myChampionAttackCooldown;

    std::array<float, 4> myEnemyHealth;

    float myBogScytheHealth;
    float myKubbLeffeHealth;
    float myCultistHealth;
    float myChampionHealth;

    Experience* myExperienceComponent;

};
