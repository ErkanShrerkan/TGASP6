#pragma once
#include <CommonUtilities/MathBundle.hpp>
#include "Engine\Stopwatch.h"
#include "Engine/NavMeshLoader.h"
#include "Transform.h"
#include <vector>

// This struct should not exist as
// it is hard coded.


struct Enemy
{

    enum class eType
    {
        eKubbLeffe,
        eBogSchyte,
        eCultist,
        eChampion,
        COUNT
    };

    Entity target;
    eType myType;
    Stopwatch* myAttackCooldown;
    Stopwatch* myFreezeCooldown;
    Stopwatch* myDeathSinkTimer;
    Stopwatch* myDeathRemoveCooldown;
    Stopwatch* myAnticipationCooldown;
    bool myCanAttack = true;

    SE::SNavTriangle* myCurrentNavTriangle;
    std::vector<int> myPathToPlayer;
    std::vector<Vector3f> myGoalVectors;
    Vector3f myPosToMoveTo;
    bool myShouldPathFind = true;
    bool myStartedWalking = false;
    bool myIsWalking = false;
    bool myHasAlreadyDied = false;
    bool myShouldMove = true;
    bool myIsAggro = false;
    bool myIsAnticipating = false;
    Transform myAnticipationTransform;
    float myAggroRange = 0.0f;

    float myMovementSpeed = 0.f;

    int myFramesLeftBeforePathfind = 1;
};
