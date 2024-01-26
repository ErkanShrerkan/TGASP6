#pragma once
#include "System.h"
#include "TriggerListener.h"
#include "Transform.h"
#include "Collider.h"
#include "TriggerListener.h"
#include "AudioSystem.h"
#include "Health.h"
#include "AttackPower.h"
#include "ModelCollection.h"
#include <array>
#include "Experience.h"
#include "Observer.h"
#include <thread>
#include "UIElement.h"

class Input;
class Stopwatch;
class AudioComponent;
class TriggerSystem;
class EnemySystem;
class MinibossSystem;
class BossSystem;
namespace SE
{
	class CCamera;
	class CModelInstance;
	struct SNavTriangle;
}

class PlayerSystem : public TriggerListener, public Observer
{

public:

	enum class Abilities
	{
		AOE,
		TELEPORT,
		HOOK,
		ULTIMATE,
		LMB,
		RMB,
		HEAL,
		COUNT
	};

	enum class AbilityDuration
	{
		AOE,
		TELEPORT,
		HOOK,
		ULTIMATE,
		LMB,
		RMB,
		HEAL,
		COUNT
	};


public:

	~PlayerSystem();

	void Init(Coordinator* aCoordinator, TriggerSystem* aTriggerSystem, EnemySystem* aLightEnemySystem, MinibossSystem* aMinibossSystem, BossSystem* aFinalBossSystem, Experience* anExperienceComponent, const int& aLevelIndex);
	void InitPlayerXP(Experience* anExperienceComponent);
	void SetNavMesh(std::vector<SE::SNavTriangle*> someNavTriangles) ;
	void SetDebugClickPos(SE::CModelInstance* aModelInstance);
	void Update();
	void FindCurrentTriangle(const Vector3f& aPosition);
	void CalculatePlayerYPosition();
	void HandleAbilityCooldowns();
	void HandleAbilityCooldownOver();
	void HandleAttackDuration();
	void MousePicking();
	void RotateToPoint(Vector2f aPos, float aLerpValue = 1.0f);
	void SetAbilityStatus(Abilities anAbility, bool aValue);
	void CheckAbilityUnlocked();
	void SetAvailableAbilites(std::array<bool, 7> someAbilities);
	void SetPlayerSpawn(Vector3f aPosition);
	void RevivePlayer();
	void StopWalking();

	bool PointIsInsideTriangle(double x, double y, double x1, double y1, double x2, double y2, double x3, double y3);
	bool RaySphereIntersect(Vector3f instance, Vector3f& p0, Vector3f& d, float r);
	bool MousePickingAndMove(const Vector3f& aRay, const bool& aShowClickVFX);
	bool GetPlayerChangedTriangleThisFrame() { return myChangedTriangleThisFrame; }
	bool TryMoveToPosition(const Vector3f& aPosToMoveTo);
	bool IsPlayerDead() { return myIsDead; }

	const float MouseHoldPosition(Vector2f aPos, SE::CCamera* aCamera, float aLerpValue);

	double CalcTriangleArea(double x1, double y1, double x2, double y2, double x3, double y3);

	Vector3f GetMouseRay();

	SE::SNavTriangle* FindPosToMoveTo(Vector3f& triangleIntersection, bool& triangleWasHit, SE::SNavTriangle* navTriangle, unsigned int& aPlayerIndex);

	SE::CModelInstance* GetModel();

	Vector4f MatrixTransform(Matrix4x4f left, Vector4f right);

	Entity GetClickedEnemy();

	Health* GetHealthComponent();

	AttackPower* GetAPComponent();

	ModelCollection* GetPlayerModel();

	SE::SNavTriangle* GetCurrentTriangle() { return myCurrentNavTriangle; }

	const Vector3f GetCurrentPosition();

	int GetCurrentXPLevel();

private:
	bool IsConeTouchingSphere(Shape& aShapeA, Transform& aTransformA, Transform& aTransformB) const;
	void RecieveMessage(eMessage aMsg);
	//VARIABLES

	Entity myPlayer;
	Health* myTargetHealth;

	std::string myComboAnims[3];

	//RAY DEBUGGING
	SE::CModelInstance* rayPoint;
	SE::CModelInstance* myDebugClickPos;

	ModelCollection* myModel;

	Transform* myLmbTarget;
	Transform* myTransform;

	ModelCollection* myLmbTargetModel;


	Experience* myExperienceComponent;
	std::thread myLevelUpThread;
	std::shared_ptr<UIElement> myLevelUpSprite;

	AttackPower* myApComponent;
	Health* myHpComponent;

	Coordinator* myCoordinator;

	TriggerSystem* myTriggerSystem;

	EnemySystem* myLightEnemySystem;

	MinibossSystem* myMinibossSystem;

	BossSystem* myFinalBossSystem = nullptr;

	Transform* myUltimateAttackTransform;

	Input* myInput;

	SE::CCamera* myCamera = nullptr;

	Vector2f myMouseDir = { 0, 1 };

	Vector3f myPosToMoveTo;
	Vector3f myJumpDestination;

	std::vector<SE::SNavTriangle*> myNavTriangles;
	std::vector<SE::CModelInstance*> myBoneDebugList;
	std::vector<Vector3f> myGoalVectors;
	std::vector<int> myPath;

	std::array<Stopwatch*, 7> myAbilityCooldowns;
	std::array<Stopwatch*, 7> myAbilityCooldownsShader;
	std::array<Stopwatch*, 7> myAbilityDurations;

	std::array<bool, 7> myAbilityAvailability;

	std::array<float, 7> myAbilityApCost;
	std::array<bool, 7> myAbilityUnlocked;

	SE::SNavTriangle* myCurrentNavTriangle;
	SE::SNavTriangle* myPrevNavTriangle;

	AudioComponent myWalkAudio;

	Stopwatch* myStopWatch;
	Stopwatch* myApRegainCooldown;
	Stopwatch* myRegainHpCooldown;
	Stopwatch* myCombatCooldown;
	Stopwatch* myDeathTimer;

	//KANSKE SKA TAS BORT? TEMP FÖR TILLFÄLLET IAF
	Stopwatch* myTakeDamageCooldown;

	bool myTriangleWasHit = false;
	bool myStartedWalking = false;
	bool myShouldMove = false;
	bool myIsWalking = false;
	bool myCanTakeDamage = true;
	bool myCanWalk = true;
	bool myChangedTriangleThisFrame = false;
	bool myIsAttacking = false;
	bool myIsDead = false;

	bool myMovementToggle = true;

	bool myAoeUnlocked = false;
	bool myHookUnlocked = false;
	bool myUltimateUnlocked = false;
	bool myTelportUnlocked = false;

	bool myFinalBossSpawned = false;
	bool myMinibossCutscenePlayed = false;
	bool myDialogCutscenePlayed = false;

	float myYPosition;
	float myTimeWalkHeldDown = 0.0f;

	//EXPOSE VARIABLES

	float myAoeDamage;
	float myUltimateDamage;
	float myRmbDamage;
	float myLmbDamage;
	float myTeleportDamage;

	float healCooldown;
	float healDuration;
	float healApCost;
	float healRestoreAmount;

	float aoeCooldown;
	float aoeDuration;
	float aoeApCost;

	float teleportCooldown;
	float teleportDuration;
	float teleportApCost;

	float hookCooldown;
	float hookDuration;
	float hookApCost;

	float ultimateCooldown;
	float ultimateDuration;
	float ultimateApCost;

	float lmbCooldown;
	float lmbDuration;
	float lmbApCost;

	float rmbCooldown;
	float rmbDuration;
	float rmbApCost;

	float movementSpeed;

	float myAPRegenAmount;

	int myCurrentComboIndex;

	Vector3f mySpawnPosition;

	//float levelToUnlockRMB;
	

	void AoeAttack();
	void Jump();
	void StandardAttack();
	void UseHealthPotion();
	void RmbAttack();
	void HookAttack();
	void LmbAttackTarget();
	void LmbAttackAir();
	void UltimateAttack();
	void UltimateAttackTrigger();
	void DebugPrints();
	void HandleApRegain();
	void HandleHpRegain();
	void HandleDamage();
	void HandleMovement();
	void RemoveAp(Abilities anAbility);	
	void HealPlayer(float anAmount);

	//WIP ATTACK THINGS
	void UltimateHover();

	virtual void OnTriggerEnter(const TriggerData& someTriggerData) override;

	bool HasEnoughAp(Abilities anAbility);
	bool IsJumpDestinationValid();

public:
	void SetTryToPlayUltimateVFX(const bool& aBool);
private:
	bool myTryToPlayUnlockUltimateVFX = false;
	int myFrameDelayCount = 0;
	int myCurrentMapLevelIndex;
};
