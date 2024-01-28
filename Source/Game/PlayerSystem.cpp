#include "pch.h"
#include "PlayerSystem.h"
#include "TriggerSystem.h"
#include "Coordinator.h"
#include "Engine\DX11.h"
#include "Engine\ContentLoader.h"
#include "Engine\Engine.h"
#include "Engine\Scene.h"
#include "Engine\Camera.h"
#include "Engine\Input.h"
#include "Engine\AudioEngine.h"
#include "Engine\PathFinding.h"
#include "Engine\ModelInstance.h"
#include "Engine\Stopwatch.h"
#include "Engine\Animator.h"
#include "UIManager.h"
#include "Engine/CDebugDrawer.h"
#include "EnemySystem.h"
#include "Expose.h"
#include "Postmaster.h"
#include "MinibossSystem.h"
#include "BossSystem.h"
#include "StateStack.h"
#include "StateCutscene.h"
#include "Engine/VFXManager.h"
#include "Light.h"
#include <Engine/PointLight.h>
#include "UIElement.h"
#include "Engine\Sprite.h"
#include "Engine/ParticleEmitter.h"
#include "Engine/ParticleEmitterInstance.h"
#include "ParticleSystem.h"
//#include "LevelHandler.h"
//#include "GameWorld.h"
//#include "Light.h"
extern SE::CCamera* _locCamera;
extern SE::CModelInstance* _locModel5;

PlayerSystem::~PlayerSystem()
{
	for (size_t i = 0; i < myAbilityCooldowns.size(); i++)
	{
		delete myAbilityCooldowns[i];
	}

	for (size_t i = 0; i < myAbilityDurations.size(); i++)
	{
		delete myAbilityDurations[i];
	}

	delete myTakeDamageCooldown;
	delete myApRegainCooldown;

	UNEXPOSE(myAoeDamage);
	UNEXPOSE(myUltimateDamage);
	UNEXPOSE(myRmbDamage);
	UNEXPOSE(myLmbDamage);
	UNEXPOSE(myTeleportDamage);
	UNEXPOSE(healCooldown);
	UNEXPOSE(healDuration);
	UNEXPOSE(healApCost);
	UNEXPOSE(healRestoreAmount);
	UNEXPOSE(aoeCooldown);
	UNEXPOSE(aoeDuration);
	UNEXPOSE(aoeApCost);
	UNEXPOSE(teleportCooldown);
	UNEXPOSE(teleportDuration);
	UNEXPOSE(teleportApCost);
	UNEXPOSE(hookCooldown);
	UNEXPOSE(hookDuration);
	UNEXPOSE(hookApCost);
	UNEXPOSE(ultimateCooldown);
	UNEXPOSE(ultimateDuration);
	UNEXPOSE(ultimateApCost);
	UNEXPOSE(lmbCooldown);
	UNEXPOSE(lmbDuration);
	UNEXPOSE(lmbApCost);
	UNEXPOSE(rmbCooldown);
	UNEXPOSE(rmbDuration);
	UNEXPOSE(rmbApCost);
	UNEXPOSE(movementSpeed);
	UNEXPOSE(myAPRegenAmount);
	//UNEXPOSE(myAoeUnlocked);
	//UNEXPOSE(myHookUnlocked);
	//UNEXPOSE(myUltimateUnlocked);
	//UNEXPOSE(myTelportUnlocked);

	Postmaster::GetInstance()->UnSubscribe(this, eMessage::eLevelUp);
	Postmaster::GetInstance()->UnSubscribe(this, eMessage::ePauseGame);
	Postmaster::GetInstance()->UnSubscribe(this, eMessage::eStartCutscene);
}

void PlayerSystem::Init(Coordinator* aCoordinator, TriggerSystem* aTriggerSystem, EnemySystem* aLightEnemySystem, MinibossSystem* aMinibossSystem, BossSystem* aFinalBossSystem, Experience* anExperienceComponent, const int& aLevelIndex)
{
	myCurrentMapLevelIndex = aLevelIndex;

	//EXPOSE //////////////////////////


	EXPOSE(myAoeDamage);
	EXPOSE(myUltimateDamage);
	EXPOSE(myRmbDamage);
	EXPOSE(myLmbDamage);
	EXPOSE(myTeleportDamage);
	EXPOSE(healCooldown);
	EXPOSE(healDuration);
	EXPOSE(healApCost);
	EXPOSE(healRestoreAmount);
	EXPOSE(aoeCooldown);
	EXPOSE(aoeDuration);
	EXPOSE(aoeApCost);
	EXPOSE(teleportCooldown);
	EXPOSE(teleportDuration);
	EXPOSE(teleportApCost);
	EXPOSE(hookCooldown);
	EXPOSE(hookDuration);
	EXPOSE(hookApCost);
	EXPOSE(ultimateCooldown);
	EXPOSE(ultimateDuration);
	EXPOSE(ultimateApCost);
	EXPOSE(lmbCooldown);
	EXPOSE(lmbDuration);
	EXPOSE(lmbApCost);
	EXPOSE(rmbCooldown);
	EXPOSE(rmbDuration);
	EXPOSE(rmbApCost);
	EXPOSE(movementSpeed);
	EXPOSE(myAPRegenAmount);

	//VARIABLES FOR ABILITY UNLOCK STATE
	//EXPOSE(myAoeUnlocked);
	//EXPOSE(myHookUnlocked);
	//EXPOSE(myUltimateUnlocked);
	//EXPOSE(myTelportUnlocked);


	//////////////////////////////////

	myCoordinator = aCoordinator;
	myTriggerSystem = aTriggerSystem;
	myLightEnemySystem = aLightEnemySystem;
	myMinibossSystem = aMinibossSystem;
	myFinalBossSystem = aFinalBossSystem;
	myInput = &Input::GetInstance();

	myComboAnims[0] = "Hit1";
	myComboAnims[1] = "Hit2";
	myComboAnims[2] = "Hit3";

	myCurrentComboIndex = 0;

	Collider myCollider;
	myCollider.shape.asSphere.radius = 75.0f;
	myCollider.shape.type = ShapeType::Sphere;
	myCollider.canCollideWithEvents.insert(TriggerEvent::LightEnemy_Ability_BasicAttack);
	myCollider.canCollideWithEvents.insert(TriggerEvent::Boss_ThirdAttack);
	myCollider.canCollideWithEvents.insert(TriggerEvent::Boss_FirstAttack);
	myCollider.canCollideWithEvents.insert(TriggerEvent::Boss_SecondAttack);
	myCollider.canCollideWithEvents.insert(TriggerEvent::Level_1);
	myCollider.canCollideWithEvents.insert(TriggerEvent::Level_2);
	myCollider.canCollideWithEvents.insert(TriggerEvent::Level_3);
	myCollider.canCollideWithEvents.insert(TriggerEvent::Level_4);
	myCollider.canCollideWithEvents.insert(TriggerEvent::Level_5);
	myCollider.canCollideWithEvents.insert(TriggerEvent::SpawnBoss);
	myCollider.canCollideWithEvents.insert(TriggerEvent::MinibossCutscene);
	myCollider.canCollideWithEvents.insert(TriggerEvent::DialogCutscene);
	myCollider.canCollideWithEvents.insert(TriggerEvent::CultistEnemy_Ability_Ranged);

	myPlayer = myCoordinator->CreateEntity();
	SE::CModel* cmodel = SE::DX11::Content->GetModelFactory().GetModel("Models/CH_PL_Pontius/CH_PL_Pontius.erc");
	ModelCollection modelCollection{ cmodel, new SE::CAnimator(cmodel) };
	modelCollection.shaderType = SE::ShaderType::eOutline;
	modelCollection.color = { .5, .75, .5, 0 };
	modelCollection.animator->AddAnimation("Models/CH_PL_Pontius/CH_PL_Death_AN.myr", "Death");
	modelCollection.animator->AddAnimation("Models/CH_PL_Pontius/CH_PL_HookingIN_AN.myr", "HookIn");
	modelCollection.animator->AddAnimation("Models/CH_PL_Pontius/CH_PL_HeavyHookAttk_AN.myr", "HeavyAttack");
	modelCollection.animator->AddAnimation("Models/CH_PL_Pontius/CH_PL_Walk_AN.myr", "Walk");
	modelCollection.animator->AddAnimation("Models/CH_PL_Pontius/CH_PL_Idle_AN.myr", "Idle");
	modelCollection.animator->AddAnimation("Models/CH_PL_Pontius/CH_PL_Heal_AN.myr", "Heal");
	modelCollection.animator->AddAnimation("Models/CH_PL_Pontius/CH_PL_LevelUp_AN.myr", "LevelUp");
	modelCollection.animator->AddAnimation("Models/CH_PL_Pontius/CH_PL_JumpStart.myr", "TeleportJump");
	modelCollection.animator->AddAnimation("Models/CH_PL_Pontius/CH_PL_JumpLand.myr", "TeleportLand");
	modelCollection.animator->AddAnimation("Models/CH_PL_Pontius/CH_EY_Pontius_HIT1_AN.myr", "Hit1");
	modelCollection.animator->AddAnimation("Models/CH_PL_Pontius/CH_EY_Pontius_HIT2_AN.myr", "Hit2");
	modelCollection.animator->AddAnimation("Models/CH_PL_Pontius/CH_EY_Pontius_HIT3_AN.myr", "Hit3");
	modelCollection.animator->SetFallbackAnimation("Idle");
	modelCollection.animator->Play("Idle", true, "Idle");
	myCoordinator->AddComponent(myPlayer, modelCollection);
	myModel = &aCoordinator->GetComponent<ModelCollection>(myPlayer);
	myCoordinator->AddComponent(myPlayer, myCollider);

	Transform aTransform{};
	aTransform.SetPosition({ -7300.f, 1600.f, -7100.f });
	myCoordinator->AddComponent(myPlayer, aTransform);

	myTransform = &myCoordinator->GetComponent<Transform>(myPlayer);

	Health aHealth;
	aHealth.Init(1000);

	myCoordinator->AddComponent(myPlayer, aHealth);

	myHpComponent = &myCoordinator->GetComponent<Health>(myPlayer);

	AttackPower anAttackPower;
	anAttackPower.Init(1000);

	myCoordinator->AddComponent(myPlayer, anAttackPower);

	myApComponent = &myCoordinator->GetComponent<AttackPower>(myPlayer);

	Singleton<UIManager>().SetPlayerVariables(&myHpComponent->GetHealth(), &myApComponent->GetAttackPower());

	BaseLight* pointLight = new BaseLight();
	pointLight->SetColor({ 1.f, 0.7f, 0.3f });
	pointLight->SetIntensity(5.0f);
	pointLight->SetRange(200.0f);
	pointLight->SetPosition(myTransform->GetPosition() + Vector3f{ 0.f, 250.f, 0.f });
	myCoordinator->AddComponent<Light>(myPlayer, pointLight);

	myCamera = SE::CEngine::GetInstance()->GetActiveScene()->GetMainCamera();

	for (size_t i = 0; i < myAbilityCooldowns.size(); i++)
	{
		myAbilityCooldowns[i] = new Stopwatch(1.f);
	}

	for (size_t i = 0; i < myAbilityCooldownsShader.size(); i++)
	{
		myAbilityCooldownsShader[i] = new Stopwatch(1.f);
	}

	for (size_t i = 0; i < myAbilityDurations.size(); i++)
	{
		myAbilityDurations[i] = new Stopwatch(10.f);
	}

	for (size_t i = 0; i < myAbilityAvailability.size(); i++)
	{
		myAbilityAvailability[i] = false;
	}

	myAbilityCooldowns[static_cast<int>(Abilities::HEAL)]->SetTimeInterval(healCooldown);
	myAbilityCooldowns[static_cast<int>(Abilities::AOE)]->SetTimeInterval(aoeCooldown);
	myAbilityCooldowns[static_cast<int>(Abilities::TELEPORT)]->SetTimeInterval(teleportCooldown);
	myAbilityCooldowns[static_cast<int>(Abilities::HOOK)]->SetTimeInterval(hookCooldown);
	myAbilityCooldowns[static_cast<int>(Abilities::ULTIMATE)]->SetTimeInterval(ultimateCooldown);
	myAbilityCooldowns[static_cast<int>(Abilities::LMB)]->SetTimeInterval(lmbCooldown);
	myAbilityCooldowns[static_cast<int>(Abilities::RMB)]->SetTimeInterval(rmbCooldown);

	myAbilityCooldownsShader[static_cast<int>(Abilities::HEAL)]->SetTimeInterval(healCooldown);
	myAbilityCooldownsShader[static_cast<int>(Abilities::AOE)]->SetTimeInterval(aoeCooldown);
	myAbilityCooldownsShader[static_cast<int>(Abilities::TELEPORT)]->SetTimeInterval(teleportCooldown);
	myAbilityCooldownsShader[static_cast<int>(Abilities::HOOK)]->SetTimeInterval(hookCooldown);
	myAbilityCooldownsShader[static_cast<int>(Abilities::ULTIMATE)]->SetTimeInterval(ultimateCooldown);
	myAbilityCooldownsShader[static_cast<int>(Abilities::LMB)]->SetTimeInterval(lmbCooldown);
	myAbilityCooldownsShader[static_cast<int>(Abilities::RMB)]->SetTimeInterval(rmbCooldown);

	myAbilityCooldownsShader[static_cast<int>(Abilities::HEAL)]->SetIsOver();
	myAbilityCooldownsShader[static_cast<int>(Abilities::AOE)]->SetIsOver();
	myAbilityCooldownsShader[static_cast<int>(Abilities::TELEPORT)]->SetIsOver();
	myAbilityCooldownsShader[static_cast<int>(Abilities::HOOK)]->SetIsOver();
	myAbilityCooldownsShader[static_cast<int>(Abilities::ULTIMATE)]->SetIsOver();
	myAbilityCooldownsShader[static_cast<int>(Abilities::LMB)]->SetIsOver();
	myAbilityCooldownsShader[static_cast<int>(Abilities::RMB)]->SetIsOver();

	myAbilityDurations[static_cast<int>(Abilities::HEAL)]->SetTimeInterval(healDuration);
	myAbilityDurations[static_cast<int>(Abilities::AOE)]->SetTimeInterval(aoeDuration);
	myAbilityDurations[static_cast<int>(Abilities::TELEPORT)]->SetTimeInterval(teleportDuration);
	myAbilityDurations[static_cast<int>(Abilities::HOOK)]->SetTimeInterval(hookDuration);
	myAbilityDurations[static_cast<int>(Abilities::ULTIMATE)]->SetTimeInterval(ultimateDuration);
	myAbilityDurations[static_cast<int>(Abilities::LMB)]->SetTimeInterval(lmbDuration);
	myAbilityDurations[static_cast<int>(Abilities::RMB)]->SetTimeInterval(rmbDuration);

	myAbilityApCost[static_cast<int>(Abilities::HEAL)] = healApCost;
	myAbilityApCost[static_cast<int>(Abilities::AOE)] = aoeApCost;
	myAbilityApCost[static_cast<int>(Abilities::TELEPORT)] = teleportApCost;
	myAbilityApCost[static_cast<int>(Abilities::HOOK)] = hookApCost;
	myAbilityApCost[static_cast<int>(Abilities::ULTIMATE)] = ultimateApCost;
	myAbilityApCost[static_cast<int>(Abilities::LMB)] = lmbApCost;
	myAbilityApCost[static_cast<int>(Abilities::RMB)] = rmbApCost;


	SetAbilityStatus(Abilities::LMB, true);
	SetAbilityStatus(Abilities::RMB, true);
	SetAbilityStatus(Abilities::HEAL, true);
	myTakeDamageCooldown = new Stopwatch(2.f);
	myApRegainCooldown = new Stopwatch(0.01f);
	myRegainHpCooldown = new Stopwatch(0.05f);
	myCombatCooldown = new Stopwatch(7.5f);
	myDeathTimer = new Stopwatch(2.f);

	myWalkAudio.SetAudioClip(AudioClip::Player_Walk);
	InitPlayerXP(anExperienceComponent);

	//POSTMASTER

	SE::CParticleEmitter* p = SE::CEngine::GetInstance()->GetContentLoader()->GetParticleFactory().GetParticleEmitter("test\0");
	ParticleEmitter en;
	myCoordinator->AddComponent<ParticleEmitter>(myPlayer, en);
	myCoordinator->GetComponent<ParticleEmitter>(myPlayer).Init(p);

	Postmaster::GetInstance()->Subscribe(this, eMessage::eLevelUp);
	Postmaster::GetInstance()->Subscribe(this, eMessage::ePauseGame);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eStartCutscene);
}

void PlayerSystem::InitPlayerXP(Experience* anExperienceComponent)
{

	myExperienceComponent = anExperienceComponent;

	/*if (myExperienceComponent->GetLevel() >= levelToUnlockRMB)
	{
		myAbilityUnlocked[static_cast<int>(Abilities::RMB)] = true;
	}*/
	if (myExperienceComponent->GetLevel() >= myExperienceComponent->GetLevelWhatLevelUnlocksAbility(static_cast<int>(Abilities::AOE)))
	{
		myAbilityUnlocked[static_cast<int>(Abilities::AOE)] = true;
		myAoeUnlocked = true;
	}
	if (myExperienceComponent->GetLevel() >= myExperienceComponent->GetLevelWhatLevelUnlocksAbility(static_cast<int>(Abilities::TELEPORT)))
	{
		myAbilityUnlocked[static_cast<int>(Abilities::TELEPORT)] = true;
		myTelportUnlocked = true;
	}
	if (myExperienceComponent->GetLevel() >= myExperienceComponent->GetLevelWhatLevelUnlocksAbility(static_cast<int>(Abilities::HOOK)))
	{
		myAbilityUnlocked[static_cast<int>(Abilities::HOOK)] = true;
		myHookUnlocked = true;
	}
	if (myExperienceComponent->GetUltimateIsUnlocked())
	{
		myAbilityUnlocked[static_cast<int>(Abilities::ULTIMATE)] = true;
		myUltimateUnlocked = true;
		myAbilityAvailability[static_cast<int>(Abilities::ULTIMATE)] = true;
	}
}

void PlayerSystem::SetNavMesh(std::vector<SE::SNavTriangle*> someNavTriangles)
{
	myNavTriangles = someNavTriangles;
}

void PlayerSystem::SetDebugClickPos(SE::CModelInstance* aModelInstance)
{
	myDebugClickPos = aModelInstance;
}

void PlayerSystem::Update()
{

	myIsDead = myCoordinator->GetComponent<Health>(myPlayer).IsEntityDead();

	if (myIsDead)
	{
		myPosToMoveTo = myTransform->GetPosition();
		myGoalVectors.clear();
		myIsWalking = false;
		myTimeWalkHeldDown = 0;
		myModel->animator->Play("Death", false);
		myModel->animator->SetFallbackAnimation("");
		Postmaster::GetInstance()->SendMail(eMessage::ePlayerDied);
		SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_Death);
	}
#ifdef _DEBUG
	if (myInput->GetInputPressed(eButtonInput::DebugLevelUp))
	{
		//FOR DEBUGGING HPBAR
	/*	auto& health = myCoordinator->GetComponent<Health>(myPlayer);

		health.TakeDamage(100);
		printf("My Health: %f\n", health.GetHealth());*/
		Postmaster::GetInstance()->SendMail(eMessage::eLevelUp);
		myExperienceComponent->AddXP(1);
	}

	if (Input::GetInputPressed(eButtonInput::ToggleOutline))
	{
		unsigned flag = (unsigned)myModel->shaderType;
		myModel = &myCoordinator->GetComponent<ModelCollection>(myPlayer);
		if (myModel->shaderType & (unsigned)SE::ShaderType::eOutline)
		{
			flag &= ~(unsigned)SE::ShaderType::eOutline;
		}
		else
		{
			flag |= (unsigned)SE::ShaderType::eOutline;
		}
		myModel->shaderType = SE::ShaderType(flag);
		myModel->color.a = 1;
	}
	if (Input::GetInputPressed(eButtonInput::ToggleHighlight))
	{
		unsigned flag = (unsigned)myModel->shaderType;
		myModel = &myCoordinator->GetComponent<ModelCollection>(myPlayer);
		if (myModel->shaderType & (unsigned)SE::ShaderType::eHighlight)
		{
			flag &= ~(unsigned)SE::ShaderType::eHighlight;
		}
		else
		{
			flag |= (unsigned)SE::ShaderType::eHighlight;
		}
		myModel->shaderType = SE::ShaderType(flag);
		myModel->color.a = 1;
	}
#endif // _DEBUG

	//EXPOSE VARIABLES LOL
	myAbilityCooldowns[static_cast<int>(Abilities::HEAL)]->SetTimeInterval(healCooldown);
	myAbilityCooldowns[static_cast<int>(Abilities::AOE)]->SetTimeInterval(aoeCooldown);
	myAbilityCooldowns[static_cast<int>(Abilities::TELEPORT)]->SetTimeInterval(teleportCooldown);
	myAbilityCooldowns[static_cast<int>(Abilities::HOOK)]->SetTimeInterval(hookCooldown);
	myAbilityCooldowns[static_cast<int>(Abilities::ULTIMATE)]->SetTimeInterval(ultimateCooldown);
	myAbilityCooldowns[static_cast<int>(Abilities::LMB)]->SetTimeInterval(lmbCooldown);
	myAbilityCooldowns[static_cast<int>(Abilities::RMB)]->SetTimeInterval(rmbCooldown);

	myAbilityDurations[static_cast<int>(Abilities::HEAL)]->SetTimeInterval(healDuration);
	myAbilityDurations[static_cast<int>(Abilities::AOE)]->SetTimeInterval(aoeDuration);
	myAbilityDurations[static_cast<int>(Abilities::TELEPORT)]->SetTimeInterval(teleportDuration);
	myAbilityDurations[static_cast<int>(Abilities::HOOK)]->SetTimeInterval(hookDuration);
	myAbilityDurations[static_cast<int>(Abilities::ULTIMATE)]->SetTimeInterval(ultimateDuration);
	myAbilityDurations[static_cast<int>(Abilities::LMB)]->SetTimeInterval(lmbDuration);
	myAbilityDurations[static_cast<int>(Abilities::RMB)]->SetTimeInterval(rmbDuration);

	myAbilityApCost[static_cast<int>(Abilities::HEAL)] = healApCost;
	myAbilityApCost[static_cast<int>(Abilities::AOE)] = aoeApCost;
	myAbilityApCost[static_cast<int>(Abilities::TELEPORT)] = teleportApCost;
	myAbilityApCost[static_cast<int>(Abilities::HOOK)] = hookApCost;
	myAbilityApCost[static_cast<int>(Abilities::ULTIMATE)] = ultimateApCost;
	myAbilityApCost[static_cast<int>(Abilities::LMB)] = lmbApCost;
	myAbilityApCost[static_cast<int>(Abilities::RMB)] = rmbApCost;

	myAbilityUnlocked[static_cast<int>(Abilities::AOE)] = myAoeUnlocked;
	myAbilityUnlocked[static_cast<int>(Abilities::HOOK)] = myHookUnlocked;
	myAbilityUnlocked[static_cast<int>(Abilities::ULTIMATE)] = myUltimateUnlocked;
	myAbilityUnlocked[static_cast<int>(Abilities::TELEPORT)] = myTelportUnlocked;

	DebugPrints();

	HandleApRegain();
	HandleDamage();
	HandleAbilityCooldownOver();
	HandleAbilityCooldowns();
	HandleAttackDuration();

	UltimateAttackTrigger();

	myWalkAudio.Update(myTransform->GetPosition());

	myCombatCooldown->Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);

	if (myCombatCooldown->IsOver())
	{
		HandleHpRegain();
	}

	if (myInput->GetInputReleased(eButtonInput::Heal))
	{
		if (myAbilityAvailability[static_cast<int>(Abilities::HEAL)])
		{
			UseHealthPotion();
		}
		else
		{
			SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_NoResource);
			//SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_NotReady);
		}
	}

	if (myIsWalking && myCanWalk)
	{
		if (myStartedWalking)
		{
			myModel->animator->Play("Walk", true);
			myStartedWalking = false;
		}
	}
	else
	{
		if (!myStartedWalking && myCanWalk)
		{
			myModel->animator->Play("Idle", true);
			myStartedWalking = true;
		}
	}

	if (myInput->GetInputPressed(eButtonInput::LBUTTON) || myInput->GetInputPressed(eButtonInput::RMBAttack))
	{
		MousePicking();
	}
	if (myInput->GetInputPressed(eButtonInput::LBUTTON))
	{
		MousePickingAndMove(GetMouseRay(), false);
	}

	Vector3f mouseRay = GetMouseRay();

	Vector3f p0 = myCamera->GetPosition() + myCamera->GetRenderOffset();
	Vector3f d = mouseRay;

	float r = 85.f;
	if (myCurrentMapLevelIndex < 4)
	{
		myLmbTarget = nullptr;

		for (auto& instance : myLightEnemySystem->myEntities)
		{
			ModelCollection* model = &myCoordinator->GetComponent<ModelCollection>(instance);
			if (!myCoordinator->GetComponent<Health>(instance).IsEntityDead())
			{
				Transform& transform = myCoordinator->GetComponent<Transform>(instance);
				CDebugDrawer::GetInstance().DrawSphere(transform.GetPosition(), 125.0f, { 0,0,1,1 });
				model->shaderType = SE::ShaderType::eNone;
				if (myCoordinator->GetComponent<Enemy>(instance).myType == Enemy::eType::eChampion)
				{
					model->shaderType = SE::ShaderType::eHighlight;
					model->color = { 0, 0, 1, 1 };
				}



				if (RaySphereIntersect(transform.GetPosition(), p0, d, r))
				{
					CDebugDrawer::GetInstance().DrawSphere(transform.GetPosition(), 125.0f, { 0,1,1,1 });
					if (myInput->GetInputPressed(eButtonInput::LBUTTON))
					{
						myLmbTarget = &transform;
						myTargetHealth = &myCoordinator->GetComponent<Health>(instance);
						myLmbTargetModel = &myCoordinator->GetComponent<ModelCollection>(instance);
						myGoalVectors.clear();
						myGoalVectors.push_back(transform.GetPosition());
						myPosToMoveTo = transform.GetPosition();
					}

					if (myCoordinator->GetComponent<Enemy>(instance).myType != Enemy::eType::eChampion)
					{
						model->shaderType = SE::ShaderType::eOutline | SE::ShaderType::eHighlight;
						model->color = { 1,0,0, 1 };
					}

				}
				else
				{
					if (myCoordinator->GetComponent<Enemy>(instance).myType != Enemy::eType::eChampion)
					{
						model->shaderType = SE::ShaderType::eNone;
					}
				}

				//USE THIS FOR OUTLINE WHEN HOVERING COLLIDER
			}
			else
			{
				model->shaderType = SE::ShaderType::eNone;
			}
		}
	}
	if (myCanWalk)
	{
		if (!myInput->GetInputHeld(eButtonInput::MovementToggle))
		{

			HandleMovement();

		}
		else
		{
			myGoalVectors.clear();
			myPosToMoveTo = myTransform->GetPosition();
			myIsWalking = false;
			myCanWalk = false;
			MouseHoldPosition(myInput->GetClampedMousePos(), myCamera, 0.4f);
			if (myInput->GetInputPressed(eButtonInput::LMBAttack))
			{
				if (myAbilityAvailability[static_cast<int>(Abilities::LMB)])
				{
					myAbilityAvailability[static_cast<int>(Abilities::LMB)] = false;
					myAbilityCooldowns[static_cast<int>(Abilities::LMB)]->Reset();
					myAbilityCooldownsShader[static_cast<int>(Abilities::LMB)]->Reset();
					myAbilityDurations[static_cast<int>(Abilities::LMB)]->Reset();

					LmbAttackAir();
				}
			}

		}
	}

	if (myFinalBossSystem != nullptr && myFinalBossSpawned)
	{
		Transform& transform = myCoordinator->GetComponent<Transform>(myFinalBossSystem->GetEntity());
		r = 550;
		CDebugDrawer::GetInstance().DrawSphere(transform.GetPosition(), 125.0f, { 0,0,1,1 });
		ModelCollection* model = &myCoordinator->GetComponent<ModelCollection>(myFinalBossSystem->GetEntity());
		model->color = { 1,0,0,1 };
		Vector3f bossPosition = transform.GetPosition() - transform.GetMatrix().GetForward() * 50.0f;
		if (RaySphereIntersect(transform.GetPosition(), p0, d, r))
		{
			CDebugDrawer::GetInstance().DrawSphere(transform.GetPosition(), 550.0f, { 0,1,1,1 });

			model->shaderType = SE::ShaderType::eHighlight;
			if (myInput->GetInputPressed(eButtonInput::LBUTTON))
			{
				myTargetHealth = &myCoordinator->GetComponent<Health>(myFinalBossSystem->GetEntity());
				myLmbTargetModel = &myCoordinator->GetComponent<ModelCollection>(myFinalBossSystem->GetEntity());
				//myLmbTarget = &transform;
				myGoalVectors.clear();
				myGoalVectors.push_back(transform.GetPosition());
				myPosToMoveTo = transform.GetPosition();
				myTimeWalkHeldDown = 0;
				myShouldMove = true;
				myIsWalking = true;
				myCanWalk = true;
			}

		}
		else
		{
			model->shaderType = SE::ShaderType::eNone;
		}
		if (myTargetHealth != nullptr && !myTargetHealth->IsEntityDead())
		{
			Vector3f distanceToEnemy = myTransform->GetPosition() - transform.GetPosition();

			float distance = 1105;
			if (distanceToEnemy.Length() < distance)
			{
				myGoalVectors.clear();
				myPosToMoveTo = myTransform->GetPosition();
				myShouldMove = false;
				myIsWalking = false;
				myStartedWalking = true;

				if (myAbilityAvailability[static_cast<int>(Abilities::LMB)] && !myInput->GetInputHeld(eButtonInput::MovementToggle))
				{
					myAbilityAvailability[static_cast<int>(Abilities::LMB)] = false;
					myAbilityCooldowns[static_cast<int>(Abilities::LMB)]->Reset();
					myAbilityCooldownsShader[static_cast<int>(Abilities::LMB)]->Reset();
					myAbilityDurations[static_cast<int>(Abilities::LMB)]->Reset();
					RotateToPoint({ transform.GetPosition().x, transform.GetPosition().z }, 1.0f);
					LmbAttackTarget();
					myTargetHealth = nullptr;
				}
			}

		}

	}
	if (myLmbTarget != nullptr)
	{
		if (!myTargetHealth->IsEntityDead())
		{

			Vector3f distanceToEnemy = myTransform->GetPosition() - myLmbTarget->GetPosition();

			float distance = 275;
			if (myCurrentMapLevelIndex == 4)
			{
				distance = 1105;
			}
			if (distanceToEnemy.Length() < distance)
			{
				myGoalVectors.clear();
				myPosToMoveTo = myTransform->GetPosition();
				myShouldMove = false;
				myIsWalking = false;
				myStartedWalking = false;

				if (myAbilityAvailability[static_cast<int>(Abilities::LMB)] && !myInput->GetInputHeld(eButtonInput::MovementToggle))
				{
					myAbilityAvailability[static_cast<int>(Abilities::LMB)] = false;
					myAbilityCooldowns[static_cast<int>(Abilities::LMB)]->Reset();
					myAbilityCooldownsShader[static_cast<int>(Abilities::LMB)]->Reset();
					myAbilityDurations[static_cast<int>(Abilities::LMB)]->Reset();
					RotateToPoint({ myLmbTarget->GetPosition().x, myLmbTarget->GetPosition().z }, 1.0f);
					LmbAttackTarget();
				}
			}

		}
	}

	if (myInput->GetInputPressed(eButtonInput::Heal))
	{
		printf("myTransform: x: %f ", myTransform->GetPosition().x);
		printf("y: %f ", myTransform->GetPosition().y);
		printf("z: %f \n", myTransform->GetPosition().z);
	}


	if (myIsWalking)
	{
		if (!myWalkAudio.GetIsPlayingOrStarting())
		{
			myWalkAudio.Start();
		}
	}
	else
	{
		if (myWalkAudio.GetIsPlayingOrStarting())
		{
			myWalkAudio.Stop(false);
		}
	}

	if (myInput->GetInputPressed(eButtonInput::Jump) && !myIsAttacking && myAbilityUnlocked[static_cast<int>(Abilities::TELEPORT)])
	{
		if (IsJumpDestinationValid())
		{
			if (HasEnoughAp(Abilities::TELEPORT))
			{
				if (myAbilityAvailability[static_cast<int>(Abilities::TELEPORT)])
				{
					Jump();
				}
				else
				{
					SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_NotReady);
				}

			}
			else
			{
				SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_NoResource);
			}
		}
	}

	if (myInput->GetInputPressed(eButtonInput::AoeAttack) && !myIsAttacking && myAbilityUnlocked[static_cast<int>(Abilities::AOE)])
	{
		if (HasEnoughAp(Abilities::AOE))
		{
			if (myAbilityAvailability[static_cast<int>(Abilities::AOE)])
			{
				AoeAttack();
			}
			else
			{
				//SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_NotReady);
			}
		}
		else
		{
			SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_NoResource);
		}
	}

	if (myInput->GetInputPressed(eButtonInput::Dammsugaren) && !myIsAttacking && myAbilityUnlocked[static_cast<int>(Abilities::HOOK)])
	{
		if (HasEnoughAp(Abilities::HOOK))
		{
			if (myAbilityAvailability[static_cast<int>(Abilities::HOOK)])
			{
				HookAttack();
			}
			else
			{
				SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_NotReady);
			}

		}
		else
		{
			SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_NoResource);
		}
	}

	if (myInput->GetInputPressed(eButtonInput::RMBAttack) && !myIsAttacking)
	{
		if (myAbilityAvailability[static_cast<int>(Abilities::RMB)] && HasEnoughAp(Abilities::RMB))
		{
			RmbAttack();
		}
	}

	if (myInput->GetInputPressed(eButtonInput::UltimateAttack) && !myIsAttacking && myAbilityUnlocked[static_cast<int>(Abilities::ULTIMATE)])
	{
		if (HasEnoughAp(Abilities::ULTIMATE))
		{
			if (myAbilityAvailability[static_cast<int>(Abilities::ULTIMATE)])
			{
				UltimateAttack();
			}
			else
			{
				SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_NotReady);
			}

		}
		else
		{
			SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_NoResource);
		}
	}


	myCamera->SetRenderOffset(myTransform->GetPosition());





	//PLAYING VFX AFTER MINIBOSS CUTSCENE DONE
	if (myTryToPlayUnlockUltimateVFX)
	{
		if (myFrameDelayCount > 0 && Singleton<UIManager>().GetStateStack().GetCurrentState().GetStateID() != eStateID::Cutscene)
		{
			Postmaster::GetInstance()->SendMail(eMessage::eUnlockUltimate);
			Transform clickEffect;
			myTryToPlayUnlockUltimateVFX = false;
			clickEffect.SetPosition(myTransform->GetPosition());
			SE::CVFXManager::GetInstance().PlayVFX("LevelUp", clickEffect);
			myModel->animator->Play("LevelUp", false);
			SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_LevelUp);
		}
		myFrameDelayCount++;
	}

}

void PlayerSystem::FindCurrentTriangle(const Vector3f& aPosition)
{

	myTriangleWasHit = false;
	myPrevNavTriangle = myCurrentNavTriangle;
	myChangedTriangleThisFrame = false;
	for (int i = 0; i < myNavTriangles.size(); i++)
	{
		if (!myTriangleWasHit)
		{
			myCurrentNavTriangle = myNavTriangles[i];
			myTriangleWasHit = PointIsInsideTriangle(aPosition.x, aPosition.z,
				myCurrentNavTriangle->myPoints[0].myPosition.x, myCurrentNavTriangle->myPoints[0].myPosition.z,
				myCurrentNavTriangle->myPoints[1].myPosition.x, myCurrentNavTriangle->myPoints[1].myPosition.z,
				myCurrentNavTriangle->myPoints[2].myPosition.x, myCurrentNavTriangle->myPoints[2].myPosition.z);
		}
	}
	if (myTriangleWasHit)
	{
		if (myPrevNavTriangle != nullptr)
		{
			if (myCurrentNavTriangle->myIndex == myPrevNavTriangle->myIndex)
			{
				myChangedTriangleThisFrame = true;
			}
		}
	}
	else
	{
		myCurrentNavTriangle = myPrevNavTriangle;
	}


}


bool PlayerSystem::TryMoveToPosition(const Vector3f& aPosToMoveTo)
{
	Vector3f prevPos = myTransform->GetPosition();

	Vector3f movement = aPosToMoveTo - prevPos;

	myTransform->SetPosition(aPosToMoveTo);

	FindCurrentTriangle(aPosToMoveTo);
	if (!myTriangleWasHit)
	{
		myTransform->SetPosition(prevPos);
		return false;
	}
	else
	{
		return true;
	}
}

void PlayerSystem::CalculatePlayerYPosition()
{
	if (myCurrentNavTriangle != nullptr)
	{
		float X = myTransform->GetPosition().x;
		float Z = myTransform->GetPosition().z;

		Vector3f vA = myCurrentNavTriangle->myPoints[0].myPosition;
		Vector3f vB = myCurrentNavTriangle->myPoints[1].myPosition;
		Vector3f vC = myCurrentNavTriangle->myPoints[2].myPosition;

		Vector3f v1 = vA - vB;
		Vector3f v2 = vA - vC;


		Vector3f n = v1.Cross(v2);

		float r = n.x; // *x
		float s = n.y; // * y
		float t = n.z; // * z

		float k = r * vA.x + s * vA.y + t * vA.z;
		float Y = (k - X * r - Z * t) / s;
		myTransform->SetPosition({ myTransform->GetPosition().x, Y, myTransform->GetPosition().z });
	}
}

void PlayerSystem::HandleAbilityCooldowns()
{
	std::array<UIElement*, 7>& abilities = Singleton<UIManager>().GetStateGameplay()->GetAbilityElements();
	for (size_t i = 0; i < myAbilityCooldowns.size(); i++)
	{
		myAbilityCooldowns[i]->Update(SE::CEngine::GetInstance()->GetDeltaTime());
		myAbilityCooldownsShader[i]->Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);
		abilities[i]->GetSprite()->SetShaderData(myAbilityCooldownsShader[i]->GetTime() / myAbilityCooldownsShader[i]->GetTimeInterval());
	}
}

void PlayerSystem::HandleAbilityCooldownOver()
{
	for (int i = 0; i < myAbilityCooldowns.size(); i++)
	{
		if (myAbilityCooldowns[i]->IsOver())
		{
			if (myAbilityUnlocked[i])
			{
				myAbilityAvailability[i] = true;
				myAbilityCooldownsShader[i]->SetIsOver();
			}
		}
	}
}


void PlayerSystem::HandleAttackDuration()
{
	myIsAttacking = false;

	for (size_t i = 0; i < myAbilityDurations.size(); i++)
	{
		myAbilityDurations[i]->Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);

		if (!myAbilityDurations[i]->IsOver())
		{
			myIsAttacking = true;
		}
	}

	if (myIsAttacking)
	{
		myIsWalking = false;
		myCanWalk = false;
	}
	else
	{
		myCanWalk = true;
	}
}

void PlayerSystem::MousePicking()
{
	Vector3f mouseRay = GetMouseRay();

	Vector3f p0 = myCamera->GetPosition() + myCamera->GetRenderOffset();
	Vector3f d = mouseRay;

	float r = 65.0f;

	//myLmbTarget = nullptr;

	//for (auto& instance : myLightEnemySystem->myEntities)
	//{
	//	if (!myCoordinator->GetComponent<Health>(instance).IsEntityDead())
	//	{
	//		Transform& transform = myCoordinator->GetComponent<Transform>(instance);
	//		CDebugDrawer::GetInstance().DrawSphere(transform.GetPosition(), 65.0f, { 0,0,1,1 });
	//		if (RaySphereIntersect(transform.GetPosition(), p0, d, r))
	//		{
	//			CDebugDrawer::GetInstance().DrawSphere(transform.GetPosition(), 65.0f, { 0,1,1,1 });
	//			myTargetHealth = &myCoordinator->GetComponent<Health>(instance);
	//			myLmbTargetModel = &myCoordinator->GetComponent<ModelCollection>(instance);
	//			myLmbTarget = &transform;
	//			ModelCollection* model = &myCoordinator->GetComponent<ModelCollection>(instance);
	//			model->shaderType = SE::ShaderType::eNone;
	//		}
	//		//USE THIS FOR OUTLINE WHEN HOVERING COLLIDER
	//	}
	//}
	if (myMinibossSystem != nullptr)
	{
		Transform& transform = myCoordinator->GetComponent<Transform>(myMinibossSystem->GetEntity());
		r = 350; //Större collider radius för minibossen
		CDebugDrawer::GetInstance().DrawSphere(transform.GetPosition(), 125.0f, { 0,0,1,1 });
		if (RaySphereIntersect(transform.GetPosition(), p0, d, r))
		{
			float length = (myTransform->GetPosition() - transform.GetPosition()).Length();
			if (length < 600.f)
			{
				CDebugDrawer::GetInstance().DrawSphere(transform.GetPosition(), 125.0f, { 0,1,1,1 });

				myMinibossSystem->TriggerEvent();

			}

		}
	}


}

Vector3f PlayerSystem::GetMouseRay()
{
	Vector3f currentRay;

	Matrix4x4f projectionMatrix;
	Matrix4x4f viewMatrix;

	viewMatrix = Matrix4x4f::GetFastInverse(myCamera->GetTransform());
	projectionMatrix = myCamera->GetProjection();

	float mouseX = myInput->GetClampedMousePos().x;
	float mouseY = myInput->GetClampedMousePos().y;
	//float resRatio = 900.0f/ 1600.0f; //TODO get resolution of window

	mouseX = ((mouseX * 2) - 1);
	mouseY = ((mouseY * 2) - 1);

	Vector4f clipCoords = { mouseX / projectionMatrix(1, 1) /*/ resRatio*/ , -mouseY / projectionMatrix(2, 2) /** resRatio*/  , -1.0f, 1.0f };

	Vector4f toEyeCoords;

	Matrix4x4f invertedProjection = Matrix4x4f::GetFastInverse(projectionMatrix);
	Vector4f eyeCoords = MatrixTransform(invertedProjection, clipCoords);

	toEyeCoords = { eyeCoords.x / projectionMatrix(1, 1) /*/ resRatio*/, /*resRatio * */eyeCoords.y / projectionMatrix(2, 2) , 1.0f, 0.0f };

	Matrix4x4f invertedViewMatrix = Matrix4x4f::GetFastInverse(viewMatrix);
	Vector4f rayWorld = MatrixTransform(invertedViewMatrix, toEyeCoords);
	return { rayWorld.x, rayWorld.y, rayWorld.z };
}

bool PlayerSystem::RaySphereIntersect(Vector3f instance, Vector3f& p0, Vector3f& d, float r)
{
	Vector3f oc = p0 - instance;
	float a = d.Dot(d);
	float b = 2 * oc.Dot(d);
	float c = oc.Dot(oc) - r * r;
	float de = b * b - 4 * a * c;

	if (de > 0)
	{
		return true;
	}
	else
	{
		return false;
	}

	//TODO: Should change that only the collider closest to the camera gets returned
}
bool PlayerSystem::MousePickingAndMove(const Vector3f& aRay, const bool& aShowClickVFX)
{
	//Vector3f mouseRay = GetMouseRay();
	myLmbTarget = nullptr;
	float distanceToIntersection;
	Vector3f triangleIntersection;

	bool triangleWasHit = false;
	SE::SNavTriangle* navTriangle;
	const int navVectorSize = int(myNavTriangles.size());
	std::vector<SE::SNavTriangle*> trianglesHit;
	// SHOULD DO SOME CULLING OF NavVectors
	for (int i = 0; i < navVectorSize; i++)
	{
		if (!triangleWasHit)
		{
			float medelY = (myNavTriangles[i]->myPoints[0].myPosition.y + myNavTriangles[i]->myPoints[1].myPosition.y + myNavTriangles[i]->myPoints[2].myPosition.y) / 3;

			distanceToIntersection = (myCamera->GetPosition().y + myCamera->GetRenderOffset().y - medelY) / -aRay.y;

			triangleIntersection = myCamera->GetPosition() + myCamera->GetRenderOffset() + (aRay * distanceToIntersection);


			navTriangle = myNavTriangles[i];
			triangleWasHit = PointIsInsideTriangle(triangleIntersection.x, triangleIntersection.z,
				navTriangle->myPoints[0].myPosition.x, navTriangle->myPoints[0].myPosition.z,
				navTriangle->myPoints[1].myPosition.x, navTriangle->myPoints[1].myPosition.z,
				navTriangle->myPoints[2].myPosition.x, navTriangle->myPoints[2].myPosition.z);


			if (triangleWasHit)
			{
				trianglesHit.push_back(navTriangle);
			}
		}
	}
	if (trianglesHit.size() == 0)
	{
		return false;
	}
	if (myCurrentMapLevelIndex >= 4 && myLmbTarget != nullptr)
	{
		return false;
	}

	SE::SNavTriangle* nearestTriangle = trianglesHit[0];
	Vector3f playerPos = { myTransform->GetPosition().x, myTransform->GetPosition().y, myTransform->GetPosition().z };
	float shortestDistanceToTriangle = (nearestTriangle->myCentroid - playerPos).LengthSqr();
	for (size_t i = 1; i < trianglesHit.size(); i++)
	{
		float distanceToTriangle = (trianglesHit[i]->myCentroid - playerPos).LengthSqr();
		if (distanceToTriangle < shortestDistanceToTriangle)
		{
			nearestTriangle = trianglesHit[i];
		}
	}

	unsigned int playerIndex;
	SE::SNavTriangle* triangleToMoveTo = FindPosToMoveTo(triangleIntersection, triangleWasHit, nearestTriangle, playerIndex);

	CDebugDrawer::GetInstance().DrawSphere(triangleIntersection, 50, { 1,1,0,1 });

	if (triangleToMoveTo == nullptr)
	{
		return false;
	}

	SE::CPathFinding pathfinder;
	myPath = pathfinder.AStar(myNavTriangles, playerIndex, nearestTriangle->myIndex, false);
	if (myPath.size() == 0 /*&& myPath[0] == i*/)
	{
		myPosToMoveTo = myTransform->GetPosition();
		return false;
	}
	std::vector<std::vector<SE::SNavVertex>> portals;

	myGoalVectors = pathfinder.Funnel(myNavTriangles, myPath, myTransform->GetPosition(), triangleIntersection);

	myShouldMove = true;

	std::reverse(myGoalVectors.begin(), myGoalVectors.end());
	if (myGoalVectors.size() > 1)
	{
		myGoalVectors.erase(std::prev(myGoalVectors.end()));
	}

	if (myGoalVectors.size() == 0 /*&& myPath[0] == i*/)
	{
		myPosToMoveTo = myTransform->GetPosition();
		return false;
	}

	if (aShowClickVFX)
	{
		Transform clickEffect;
		clickEffect.SetPosition({ triangleIntersection.x, triangleIntersection.y + 10.0f,triangleIntersection.z });
		SE::CVFXManager::GetInstance().PlayVFX("NavmeshClick", clickEffect);
	}

	return true;
}

SE::SNavTriangle* PlayerSystem::FindPosToMoveTo(Vector3f& triangleIntersection, bool& triangleWasHit, SE::SNavTriangle* navTriangle, unsigned int& aPlayerIndex)
{
	aPlayerIndex;
	Vector3f posToMoveTo;

	myDebugClickPos->SetPosition(triangleIntersection);
	myShouldMove = true;
	triangleWasHit = true;

	bool playerInTriangle = false;
	SE::SNavTriangle* playerNavTriangle = nullptr;
	for (int j = 0; j < myNavTriangles.size(); j++)
	{
		playerNavTriangle = myNavTriangles[j];
		playerInTriangle = PointIsInsideTriangle(myTransform->GetPosition().x, myTransform->GetPosition().z,
			playerNavTriangle->myPoints[0].myPosition.x, playerNavTriangle->myPoints[0].myPosition.z,
			playerNavTriangle->myPoints[1].myPosition.x, playerNavTriangle->myPoints[1].myPosition.z,
			playerNavTriangle->myPoints[2].myPosition.x, playerNavTriangle->myPoints[2].myPosition.z);
		if (playerInTriangle)
		{
			aPlayerIndex = playerNavTriangle->myIndex;
			break;
		}
	}

	if (playerInTriangle)
	{
		return navTriangle;
	}
	return nullptr;
}

const float PlayerSystem::MouseHoldPosition(Vector2f aPos, SE::CCamera* aCamera, float aLervValue)
{
	float lerpRotate = aLervValue;
	float PI = 3.141592653589793238463f;
	Matrix4x4f cameraTransform(myCamera->GetTransform());
	float4 cameraPos(myCamera->GetPosition() + myCamera->GetRenderOffset(), 1);
	cameraTransform.SetRow(4, cameraPos);

	Vector4f viewPosition = myTransform->GetTransform().GetRow(4) * Matrix4x4f::GetFastInverse(cameraTransform);
	Vector4f projectionPosition = viewPosition * myCamera->GetProjection();
	projectionPosition = projectionPosition / projectionPosition.w;


	float screenX = ((projectionPosition.x * (16.f / 9)) + 1) * 0.5f;
	float screenY = 1 - (projectionPosition.y + 1) * 0.5f;
	myMouseDir = aPos - Vector2f{ screenX, screenY }; //{0.5f, 0.5f} är om spelaren är i mitten av skärmen



	float mouseRotateCos = myMouseDir.Dot({ 0,1 }) / (myMouseDir.Length() * 1);
	float mouseRotateAngleRad = acos(mouseRotateCos);
	float mouseRotateAngleDeg = mouseRotateAngleRad * 180 / PI;

	if (myMouseDir.x < 0)
	{
		mouseRotateAngleDeg = 360 - mouseRotateAngleDeg;
	}

	Vector2f playerDir;
	playerDir = { -myTransform->GetTransform()(3, 1), myTransform->GetTransform()(3, 3) };


	float playerRotateCos = playerDir.Dot({ 0,1 }) / (playerDir.Length() * 1);
	float playerRotateAngleRad = acos(playerRotateCos);
	float playerRotateAngleDeg = playerRotateAngleRad * 180 / PI;
	playerRotateAngleDeg;
	if (playerDir.x < 0)
	{
		playerRotateAngleDeg = 360 - playerRotateAngleDeg;
	}


	Vector2f cameraDir;
	cameraDir = { -aCamera->GetTransform()(3, 1), aCamera->GetTransform()(3, 3) };
	float cameraRotateCos = cameraDir.Dot({ 0,1 }) / (cameraDir.Length() * 1);
	float cameraRotateAngleRad = acos(cameraRotateCos);
	float cameraRotateAngleDeg = cameraRotateAngleRad * 180 / PI;
	if (cameraDir.x > 0)
	{
		cameraRotateAngleDeg = 360 - cameraRotateAngleDeg;
	}


	float angle = playerRotateAngleDeg - mouseRotateAngleDeg + cameraRotateAngleDeg;

	if (angle >= 180.0f)
	{
		angle = 360 - angle;
		angle = -angle;
	}
	else if (angle < -180.0f)
	{
		angle = 360 + angle;
	}
	myTransform->Rotate({ 0,angle * lerpRotate,0 });
	return angle;
}

void PlayerSystem::RotateToPoint(Vector2f aPos, float aLerpValue)
{
	float PI = 3.141592653589793238463f;
	myMouseDir = aPos - Vector2f{ myTransform->GetPosition().x, myTransform->GetPosition().z };
	myMouseDir.Normalize();


	float mouseRotateCos = myMouseDir.Dot({ 0,-1 }) / (myMouseDir.Length() * 1);
	float mouseRotateAngleRad = acos(mouseRotateCos);
	float mouseRotateAngleDeg = mouseRotateAngleRad * 180 / PI;

	if (myMouseDir.x < 0)
	{
		mouseRotateAngleDeg = 360 - mouseRotateAngleDeg;
	}

	Vector2f playerDir;
	playerDir = { -myTransform->GetTransform()(3, 1), myTransform->GetTransform()(3, 3) };

	float playerRotateCos = playerDir.Dot({ 0,1 }) / (playerDir.Length() * 1);
	float playerRotateAngleRad = acos(playerRotateCos);
	float playerRotateAngleDeg = playerRotateAngleRad * 180 / PI;

	if (playerDir.x < 0)
	{
		playerRotateAngleDeg = 360 - playerRotateAngleDeg;
	}
	float angle = playerRotateAngleDeg - mouseRotateAngleDeg;

	if (angle >= 180.0f)
	{
		angle = 360 - angle;
		angle = -angle;
	}
	else if (angle < -180.0f)
	{
		angle = 360 + angle;
	}
	myTransform->Rotate({ 0, angle * aLerpValue, 0 });
}

SE::CModelInstance* PlayerSystem::GetModel()
{
	return nullptr;
	//return myModel;
}

double PlayerSystem::CalcTriangleArea(double x1, double y1, double x2, double y2, double x3, double y3)
{
	return abs(x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0;
}

bool PlayerSystem::PointIsInsideTriangle(double x, double y, double x1, double y1, double x2, double y2, double x3, double y3)
{
	/* Calculate area of triangle ABC */
	double A = CalcTriangleArea(x1, y1, x2, y2, x3, y3);

	/* Calculate area of triangle PBC */
	double A1 = CalcTriangleArea(x, y, x2, y2, x3, y3);

	/* Calculate area of triangle PAC */
	double A2 = CalcTriangleArea(x1, y1, x, y, x3, y3);

	/* Calculate area of triangle PAB */
	double A3 = CalcTriangleArea(x1, y1, x2, y2, x, y);

	/* Check if sum of A1, A2 and A3 is same as A */
	return (A == A1 + A2 + A3);
}

Vector4f PlayerSystem::MatrixTransform(Matrix4x4f left, Vector4f right)
{
	Vector4f dest;

	float x = left(1, 1) * right.x + left(2, 1) * right.y + left(3, 1) * right.z + left(4, 1) * right.w;
	float y = left(1, 2) * right.x + left(2, 2) * right.y + left(3, 2) * right.z + left(4, 2) * right.w;
	float z = left(1, 3) * right.x + left(2, 3) * right.y + left(3, 3) * right.z + left(4, 3) * right.w;
	float w = left(1, 4) * right.x + left(2, 4) * right.y + left(3, 4) * right.z + left(4, 4) * right.w;

	dest.x = x;
	dest.y = y;
	dest.z = z;
	dest.w = w;

	return dest;
}

void PlayerSystem::AoeAttack()
{
	myStartedWalking = true;

	SE::CEngine::GetInstance()->GetActiveScene()->GetMainCamera()->Shake(5.0f, 0.25f);

	myAbilityAvailability[static_cast<int>(Abilities::AOE)] = false;
	myAbilityCooldowns[static_cast<int>(Abilities::AOE)]->Reset();
	myAbilityCooldownsShader[static_cast<int>(Abilities::AOE)]->Reset();
	myAbilityDurations[static_cast<int>(Abilities::AOE)]->Reset();


	RemoveAp(Abilities::AOE);

	myModel->animator->Play("Heal", false, "Idle");

	Transform t;
	t.SetPosition({ myTransform->GetPosition().x, myTransform->GetPosition().y + 15.f, myTransform->GetPosition().z });
	SE::CVFXManager::GetInstance().PlayVFX("AOE", t);

	Trigger myAoeTrigger;
	myAoeTrigger.event = TriggerEvent::Player_Ability_AOE;
	myAoeTrigger.shape.asSphere.radius = 400.f;
	myAoeTrigger.shape.type = ShapeType::Sphere;
	myAoeTrigger.damage = myAoeDamage;
	Transform transform;
	transform.SetPosition(myTransform->GetPosition());

	myTriggerSystem->CreateTrigger(myAoeTrigger, transform);

	SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_Ability1, (myTransform->GetPosition()));
}

void PlayerSystem::HookAttack()
{
	myStartedWalking = true;

	MouseHoldPosition(myInput->GetClampedMousePos(), myCamera, 1.0f);

	myAbilityAvailability[static_cast<int>(Abilities::HOOK)] = false;
	myAbilityCooldowns[static_cast<int>(Abilities::HOOK)]->Reset();
	myAbilityCooldownsShader[static_cast<int>(Abilities::HOOK)]->Reset();
	myAbilityDurations[static_cast<int>(Abilities::HOOK)]->Reset();
	//Singleton<UIManager>().SetPlayerAbilityGreyedStatus(true, 2);

	RemoveAp(Abilities::HOOK);

	myModel->animator->Play("HookIn", false, "Idle");

	Trigger myHookTrigger;
	myHookTrigger.event = TriggerEvent::Player_Ability_Dammsugare;
	myHookTrigger.shape.asSphere.radius = 700.f;
	myHookTrigger.shape.type = ShapeType::Cone;
	myHookTrigger.lifeTime = .75f;
	myHookTrigger.shape.asCone.angle = 60.f;

	float3 distance = (myTransform->GetTransform().GetPosition() - myTransform->GetTransform().GetForward() - myTransform->GetPosition());
	float2 relPoint = { distance.x, distance.z };
	myHookTrigger.shape.asCone.x = relPoint.x;
	myHookTrigger.shape.asCone.y = relPoint.y;

	Transform transform;
	transform.SetPosition(myTransform->GetPosition() + float3(0, 25, 0));
	transform.GetTransform().SetRotation(myTransform->GetTransform());
	SE::CVFXManager::GetInstance().PlayVFX("Hook", transform);

	SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_Ability3, (myTransform->GetPosition()));


#ifdef _DEBUG
	std::vector<Transform> spheres;

	for (int i = -16; i < 16; i++)
	{
		for (int j = -16; j < 16; j++)
		{
			float4 variabelnamn = { i * 100.f + myTransform->GetPosition().x, myTransform->GetPosition().y + 100.f, j * 200.f + myTransform->GetPosition().z , 1 };
			Transform transa;
			transa.GetTransform().SetRow(4, variabelnamn);
			spheres.push_back(transa);

		}
	}

	for (auto& sphere : spheres)
	{
		if (IsConeTouchingSphere(myHookTrigger.shape, transform, sphere))
		{
			CDebugDrawer::GetInstance().DrawSphere(sphere.GetTransform().GetRow(4).xyz, 20.f, { 1.f, 0.f, 1.f, 1.f });
		}
	}
#endif // _DEBUG

	myTriggerSystem->CreateTrigger(myHookTrigger, transform);
}

bool PlayerSystem::IsJumpDestinationValid()
{
	Vector3f mouseRay = GetMouseRay();

	float distanceToIntersection;
	Vector3f triangleIntersection;

	Vector3f vector = { 0,0,0 };

	bool triangleWasHit = false;
	SE::SNavTriangle* navTriangle;
	const int navVectorSize = int(myNavTriangles.size());

	// SHOULD DO SOME CULLING OF NavVectors
	for (int i = 0; i < navVectorSize; i++)
	{
		if (!triangleWasHit)
		{
			float medelY = (myNavTriangles[i]->myPoints[0].myPosition.y + myNavTriangles[i]->myPoints[1].myPosition.y + myNavTriangles[i]->myPoints[2].myPosition.y) / 3;

			distanceToIntersection = (myCamera->GetPosition().y + myCamera->GetRenderOffset().y - medelY) / -mouseRay.y;

			triangleIntersection = myCamera->GetPosition() + myCamera->GetRenderOffset() + (mouseRay * distanceToIntersection);


			navTriangle = myNavTriangles[i];
			triangleWasHit = PointIsInsideTriangle(triangleIntersection.x, triangleIntersection.z,
				navTriangle->myPoints[0].myPosition.x, navTriangle->myPoints[0].myPosition.z,
				navTriangle->myPoints[1].myPosition.x, navTriangle->myPoints[1].myPosition.z,
				navTriangle->myPoints[2].myPosition.x, navTriangle->myPoints[2].myPosition.z);


			if (triangleWasHit)
			{
				myJumpDestination = { triangleIntersection.x, triangleIntersection.y, triangleIntersection.z };
				return true;
			}
		}
	}

	return false;
}

void PlayerSystem::SetTryToPlayUltimateVFX(const bool& aBool)
{
	myTryToPlayUnlockUltimateVFX = aBool;
}

void PlayerSystem::Jump()
{

	SE::CEngine::GetInstance()->GetActiveScene()->GetMainCamera()->Shake(5.0f, 0.25f);

	myStartedWalking = true;
	//Singleton<UIManager>().SetPlayerAbilityGreyedStatus(true, 1);

	myAbilityAvailability[static_cast<int>(Abilities::TELEPORT)] = false;
	myAbilityCooldowns[static_cast<int>(Abilities::TELEPORT)]->Reset();
	myAbilityCooldownsShader[static_cast<int>(Abilities::TELEPORT)]->Reset();
	myAbilityDurations[static_cast<int>(Abilities::TELEPORT)]->Reset();
	RemoveAp(Abilities::TELEPORT);
	MouseHoldPosition(myInput->GetClampedMousePos(), myCamera, 1.f);

	SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_JumpStart);
	myTransform->SetPosition(myJumpDestination);
	Transform t;
	t.SetPosition({ myTransform->GetPosition().x, myTransform->GetPosition().y + 25.f, myTransform->GetPosition().z });
	SE::CVFXManager::GetInstance().PlayVFX("Teleport", t);
	FindCurrentTriangle(myJumpDestination);
	SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_JumpLand);

	Trigger myAttackTrigger;
	myAttackTrigger.event = TriggerEvent::Player_Ability_Teleport;
	myAttackTrigger.shape.asSphere.radius = 250.f;
	myAttackTrigger.shape.type = ShapeType::Sphere;
	myAttackTrigger.damage = myTeleportDamage;
	Transform transform;
	transform.SetPosition(myTransform->GetPosition());

	myModel->animator->Play("TeleportLand", false, "Idle");

	myTriggerSystem->CreateTrigger(myAttackTrigger, transform);

	myGoalVectors.clear();
	myPosToMoveTo = myTransform->GetPosition();
	myIsWalking = false;
	myStartedWalking = false;
}

void PlayerSystem::StandardAttack()
{
}

void PlayerSystem::UseHealthPotion()
{
	myStartedWalking = true;
	myIsWalking = false;


	HealPlayer(healRestoreAmount);
	myAbilityAvailability[static_cast<int>(Abilities::HEAL)] = false;
	myAbilityCooldowns[static_cast<int>(Abilities::HEAL)]->Reset();
	myAbilityCooldownsShader[static_cast<int>(Abilities::HEAL)]->Reset();
	myAbilityDurations[static_cast<int>(Abilities::HEAL)]->Reset();
	RemoveAp(Abilities::HEAL);
	SE::CVFXManager::GetInstance().PlayVFX("Heal", *myTransform);

	myModel->animator->Play("Heal", false, "Idle");

	SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_Heal);
}

void PlayerSystem::RmbAttack()
{
	myStartedWalking = true;

	myAbilityAvailability[static_cast<int>(Abilities::RMB)] = false;
	myAbilityCooldowns[static_cast<int>(Abilities::RMB)]->Reset();
	myAbilityCooldownsShader[static_cast<int>(Abilities::RMB)]->Reset();
	myAbilityDurations[static_cast<int>(Abilities::RMB)]->Reset();
	//Singleton<UIManager>().SetPlayerAbilityGreyedStatus(true, 5);

	RemoveAp(Abilities::RMB);

	MouseHoldPosition(myInput->GetClampedMousePos(), myCamera, 1.f);

	SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_RMBAttack);

	myModel->animator->Play("HeavyAttack", false, "Idle", false, 0.1f, 0.3f);

	//Transform t;
	//t.SetPosition(myTransform->GetPosition() - 1.f * myTransform->GetTransform().GetForward());
	//t.GetTransform().SetRotation(myTransform->GetTransform());
	SE::CVFXManager::GetInstance().PlayVFX("RMB", *myTransform);

	Trigger myAttackTrigger;
	myAttackTrigger.event = TriggerEvent::Player_Ability_RmbAttack;
	myAttackTrigger.shape.asSphere.radius = 150.f;
	myAttackTrigger.shape.type = ShapeType::Sphere;
	myAttackTrigger.damage = myRmbDamage;
	Transform transform;
	transform.SetPosition(myTransform->GetPosition() - 180.f * myTransform->GetTransform().GetForward());

	myTriggerSystem->CreateTrigger(myAttackTrigger, transform);
}

void PlayerSystem::LmbAttackAir()
{
	if (!myHitAnimIsDone)
	{
		return;
	}

	myStartedWalking = true;

	myAbilityAvailability[static_cast<int>(Abilities::LMB)] = false;
	myAbilityCooldowns[static_cast<int>(Abilities::LMB)]->Reset();
	myAbilityDurations[static_cast<int>(Abilities::LMB)]->Reset();
	//Singleton<UIManager>().SetPlayerAbilityGreyedStatus(true, 4);

	RemoveAp(Abilities::LMB);

	MouseHoldPosition(myInput->GetClampedMousePos(), myCamera, 1.f);

	if (myCurrentComboIndex > 2)
	{
		myCurrentComboIndex = 0;
	}

	myModel->animator->Play(myComboAnims[myCurrentComboIndex], false, "Idle", false, 0.1f, 0.15f);
	myHitAnimIsDone = false;
	myModel->animator->SetOnOverCallback([&]()
		{
			myHitAnimIsDone = true;
			myCurrentComboIndex++;
		});

	SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_LMBAttack);

	//myModel->animator->Play("HeavyAttack", false, "Idle");

	//Transform t;
	//t.SetPosition(myTransform->GetPosition() - 1.f * myTransform->GetTransform().GetForward());
	//t.GetTransform().SetRotation(myTransform->GetTransform());
	SE::CVFXManager::GetInstance().PlayVFX("RMB", *myTransform);

	Trigger myAttackTrigger;
	myAttackTrigger.event = TriggerEvent::Player_Ability_Lmb;
	myAttackTrigger.shape.asSphere.radius = 80.f;
	myAttackTrigger.shape.type = ShapeType::Sphere;
	myAttackTrigger.damage = myLmbDamage;
	Transform transform;
	transform.SetPosition(myTransform->GetPosition() - 180.f * myTransform->GetTransform().GetForward());

	myTriggerSystem->CreateTrigger(myAttackTrigger, transform);
}

void PlayerSystem::LmbAttackTarget()
{
	if (!myHitAnimIsDone)
	{
		return;
	}

	SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_LMBAttack);
	myStartedWalking = true;

	//RotateToPoint(myLmbTarget->GetPosition().xy, 5.f);

	RemoveAp(Abilities::LMB);

	//myModel->animator->Play("HeavyAttack", false, "Idle");

	if (myCurrentComboIndex > 2)
	{
		myCurrentComboIndex = 0;
	}

	SE::CVFXManager::GetInstance().PlayVFX("RMB", *myTransform);
	myModel->animator->Play(myComboAnims[myCurrentComboIndex], false, "Idle", false, 0.1f, 0.15f);
	myHitAnimIsDone = false;
	myModel->animator->SetOnOverCallback([&]()
		{
			myHitAnimIsDone = true;
			myCurrentComboIndex++;
		});

	myTargetHealth->TakeDamage(myLmbDamage);

	myCoordinator->GetComponent<AttackPower>(myPlayer).RegainAttackPower(25);

	if (myCurrentMapLevelIndex < 4)
	{
		myLmbTargetModel->animator->Play("Hit", false, "Idle");
	}

	myShouldMove = false;
	myPosToMoveTo = myTransform->GetPosition();
	myGoalVectors.clear();
	myTimeWalkHeldDown = 0;
	myLmbTarget = nullptr;
}

void PlayerSystem::UltimateAttack()
{
	myStartedWalking = true;

	SE::CEngine::GetInstance()->GetActiveScene()->GetMainCamera()->Shake(5.0f, 0.25f);

	myAbilityAvailability[static_cast<int>(Abilities::ULTIMATE)] = false;
	myAbilityCooldowns[static_cast<int>(Abilities::ULTIMATE)]->Reset();
	myAbilityCooldownsShader[static_cast<int>(Abilities::ULTIMATE)]->Reset();
	myAbilityDurations[static_cast<int>(Abilities::ULTIMATE)]->Reset();
	//Singleton<UIManager>().SetPlayerAbilityGreyedStatus(true, 3);
	RemoveAp(Abilities::ULTIMATE);

	MouseHoldPosition(myInput->GetClampedMousePos(), myCamera, 1.f);
	//RotateToPoint(myInput->GetClampedMousePos());
	Transform t;
	t.SetPosition(myTransform->GetPosition() - 150.f * myTransform->GetTransform().GetForward());
	t.Move({ 0, 200, 0 });
	t.GetTransform().SetRotation(myTransform->GetTransform());
	SE::CVFXManager::GetInstance().PlayVFX("Ultimate", t);

	myModel->animator->Play("LevelUp", false, "Idle");


	SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_Ability4);
	for (size_t i = 0; i < 7; i++)
	{
		Trigger trigger;
		trigger.event = TriggerEvent::Player_Ability_Ultimate;
		trigger.shape.asSphere.radius = 250.f;
		trigger.shape.type = ShapeType::Sphere;
		trigger.lifeTime = 1.f;
		trigger.damage = myUltimateDamage;
		Transform transform;

		if (i == 0)
		{
			transform.SetPosition((myTransform->GetPosition() - 320.f * myTransform->GetTransform().GetForward()));
		}
		else if (i > 1)
		{
			transform.SetPosition((myTransform->GetPosition() - trigger.shape.asSphere.radius * 1.2f * i * myTransform->GetTransform().GetForward()));
		}

		myTriggerSystem->CreateTrigger(trigger, transform);
	}

	/*Trigger myUltimateAttackTrigger;
	myUltimateAttackTrigger.event = TriggerEvent::Player_Ability_Ultimate;
	myUltimateAttackTrigger.shape.asSphere.radius = 500.f;
	myUltimateAttackTrigger.shape.type = ShapeType::Sphere;
	myUltimateAttackTrigger.lifeTime = 1.f;

	Transform transform = *myTransform;

	Entity utrigger = myTriggerSystem->CreateTrigger(myUltimateAttackTrigger, transform);
	myUltimateAttackTransform = &myCoordinator->GetComponent<Transform>(utrigger);*/
}

void PlayerSystem::HandleApRegain()
{
	if (myApComponent->GetAttackPower() < myApComponent->GetMaxAttackPower())
	{
		myApRegainCooldown->Update(SE::CEngine::GetInstance()->GetDeltaTime());

		if (myApRegainCooldown->IsOver())
		{
			myApComponent->RegainAttackPower(myAPRegenAmount);
		}
	}
}

void PlayerSystem::HandleHpRegain()
{
	if (myHpComponent->GetHealth() < myHpComponent->GetMaxHealth())
	{
		myRegainHpCooldown->Update(SE::CEngine::GetInstance()->GetDeltaTime());

		if (myRegainHpCooldown->IsOver())
		{
			myHpComponent->Heal(1);
		}
	}
}

void PlayerSystem::HealPlayer(float anAmount)
{
	myHpComponent->Heal(anAmount);
}

void PlayerSystem::RevivePlayer()
{
	myTransform->SetPosition(mySpawnPosition);
	myModel->animator->Play("LevelUp", false, "Idle");
	SE::CVFXManager::GetInstance().PlayVFX("Heal", *myTransform);
	myModel->animator->SetFallbackAnimation("Idle");

	myPosToMoveTo = myTransform->GetPosition();
	myLmbTarget = nullptr;

	myCoordinator->GetComponent<Health>(myPlayer).ResetHealth();
	myCoordinator->GetComponent<AttackPower>(myPlayer).ResetAttackPower();
	SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_Revive);
}

void PlayerSystem::StopWalking()
{
	if (Singleton<UIManager>().GetStateStack().GetCurrentState().GetStateID() == eStateID::Pause || Singleton<UIManager>().GetStateStack().GetCurrentState().GetStateID() == eStateID::Cutscene)
	{
		myModel->animator->Play("Idle", true);
		myIsWalking = false;
	}
}

void PlayerSystem::SetAvailableAbilites(std::array<bool, 7> someAbilities)
{
	for (int i = 0; i < myAbilityUnlocked.size(); i++)
	{
		myAbilityUnlocked[i] = someAbilities[i];
		myAbilityCooldowns[i]->SetIsOver();
		Singleton<UIManager>().SetPlayerAbilityLockedStatus(!someAbilities[i], i);
		switch (static_cast<Abilities>(i))
		{
		case Abilities::AOE:
			if (myAbilityUnlocked[i])
			{
				myAoeUnlocked = true;
			}
			break;
		case Abilities::TELEPORT:
			if (myAbilityUnlocked[i])
			{
				myTelportUnlocked = true;
			}
			break;
		case Abilities::HOOK:
			if (myAbilityUnlocked[i])
			{
				myHookUnlocked = true;
			}
			break;
		case Abilities::ULTIMATE:
			if (myAbilityUnlocked[i])
			{
				myUltimateUnlocked = true;
			}
			break;
		default:
			break;
		}
	}
}

void PlayerSystem::SetPlayerSpawn(Vector3f aPosition)
{
	mySpawnPosition = aPosition;
}

void PlayerSystem::SetAbilityStatus(Abilities anAbility, bool aValue)
{
	if (anAbility == Abilities::ULTIMATE)
	{
		myUltimateUnlocked = true;
		myExperienceComponent->SetUltimateIsUnlocked(true);
	}
	myAbilityUnlocked[static_cast<int>(anAbility)] = aValue;
	Singleton<UIManager>().SetPlayerAbilityLockedStatus(false, static_cast<int>(anAbility));
	myAbilityCooldowns[static_cast<int>(anAbility)]->SetIsOver();
}

void PlayerSystem::CheckAbilityUnlocked()
{
	for (int i = 0; i < myAbilityUnlocked.size(); i++)
	{
		if (!myAbilityUnlocked[i])
		{
			Singleton<UIManager>().SetPlayerAbilityLockedStatus(true, i);
		}
	}
}

void PlayerSystem::DebugPrints()
{
}

void PlayerSystem::UltimateAttackTrigger()
{
	if (!myAbilityDurations[static_cast<int>(Abilities::ULTIMATE)]->IsOver())
	{
		if (myUltimateAttackTransform != nullptr)
		{
			Trigger myUltimateAttackTrigger;
			myUltimateAttackTrigger.event = TriggerEvent::Player_Ability_Ultimate;
			myUltimateAttackTrigger.shape.asSphere.radius = 500.f;
			myUltimateAttackTrigger.shape.type = ShapeType::Sphere;
			myUltimateAttackTrigger.lifeTime = 1.f;

			Transform transform = *myTransform;

			Entity utrigger = myTriggerSystem->CreateTrigger(myUltimateAttackTrigger, transform);
			myUltimateAttackTransform = &myCoordinator->GetComponent<Transform>(utrigger);

			myUltimateAttackTransform->Move({ 0, 0, -30.f });
		}
	}
}

void PlayerSystem::HandleDamage()
{
	if (!myCanTakeDamage)
	{
		myTakeDamageCooldown->Update(SE::CEngine::GetInstance()->GetDeltaTime());

		if (myTakeDamageCooldown->IsOver())
		{
			myCanTakeDamage = true;
		}
	}
}

void PlayerSystem::HandleMovement()
{
	Vector2f posDif;
	Vector3f playerPos = myTransform->GetPosition();

	Vector2f difPlayerPos2 = { myTransform->GetPosition().x, myTransform->GetPosition().z };
	Vector2f difPosToMoveTo2 = { myPosToMoveTo.x, myPosToMoveTo.z };

	myCoordinator->GetComponent<Light>(myPlayer)->SetPosition(playerPos + Vector3f{ 0.f, 250.f, 0.f });

	posDif = difPosToMoveTo2 - difPlayerPos2;

	//myIsWalking = false;
	if (posDif.LengthSqr() > 10.0f && myShouldMove)
	{
		Vector3f prevPos = myTransform->GetPosition();

		myIsWalking = true;
		RotateToPoint({ myPosToMoveTo.x, myPosToMoveTo.z }, 1.0f); //If this lerpvalue is too low, the bug may occur where player runs around in circles.
		myTransform->Move({ 0,0, -movementSpeed * SE::CEngine::GetInstance()->GetDeltaTime() });
		FindCurrentTriangle(myTransform->GetPosition());

		if (!myGoalVectors.empty() && (myGoalVectors[0] - playerPos).Length() < 50.0f)
		{
			myIsWalking = false;
			myShouldMove = false;
		}
	}

	if (myInput->GetInputDown(eButtonInput::LBUTTON) || myInput->GetInputDown(eButtonInput::PlayerWalk))
	{
		myTimeWalkHeldDown += SE::CEngine::GetInstance()->GetDeltaTime();
		if (myTimeWalkHeldDown >= 0.5f)
		{
			myIsWalking = true;
			myShouldMove = false;
			myPosToMoveTo = myTransform->GetPosition();
			Vector3f prevPos = myPosToMoveTo;

			myTransform->Move({ 0, 0, -movementSpeed * SE::CEngine::GetInstance()->GetDeltaTime() });

			FindCurrentTriangle(myTransform->GetPosition());
			if (!myInput->GetInputDown(eButtonInput::Down) && !myTriangleWasHit)
			{
				myTransform->SetPosition(prevPos);
				FindCurrentTriangle(prevPos);
			}
			else
			{
				myPosToMoveTo = myDebugClickPos->GetPosition();
			}

			MouseHoldPosition(myInput->GetClampedMousePos(), myCamera, 0.4f);
		}

	}

	if (myInput->GetInputReleased(eButtonInput::LBUTTON) || myInput->GetInputReleased(eButtonInput::PlayerWalk)/* && myTimeWalkHeldDown >= 0.5f*/)
	{
		myTimeWalkHeldDown = 0;
	}

	if (myTimeWalkHeldDown == 0 && !myShouldMove)
	{
		myIsWalking = false;
	}

	if (myLmbTarget == nullptr && (myInput->GetInputPressed(eButtonInput::LBUTTON) || myInput->GetInputPressed(eButtonInput::PlayerWalk)))
	{
		MousePickingAndMove(GetMouseRay(), true);
	}

	if (myIsWalking && myTriangleWasHit)
	{
		CalculatePlayerYPosition();
	}

	int size = (int)myGoalVectors.size();
	if (size > 0)
	{
		myPosToMoveTo = myGoalVectors[myGoalVectors.size() - 1];

		Vector2f playerPos2 = { myTransform->GetPosition().x, myTransform->GetPosition().z };
		Vector2f posToMoveTo2 = { myPosToMoveTo.x, myPosToMoveTo.z };

		if ((posToMoveTo2 - playerPos2).LengthSqr() < 500.0f)
		{
			myGoalVectors.pop_back();
		}
	}
	else
	{
		//myPosToMoveTo = myDebugClickPos->GetPosition();
	}

}

Entity PlayerSystem::GetClickedEnemy()
{
	return Entity();
}

Health* PlayerSystem::GetHealthComponent()
{
	return myHpComponent;
}

AttackPower* PlayerSystem::GetAPComponent()
{
	return myApComponent;
}

ModelCollection* PlayerSystem::GetPlayerModel()
{
	return myModel;
}

void PlayerSystem::RemoveAp(Abilities anAbility)
{
	myApComponent->DrainAttackPower(myAbilityApCost[static_cast<int>(anAbility)]);
}

bool PlayerSystem::HasEnoughAp(Abilities anAbility)
{
	return myApComponent->GetAttackPower() >= myAbilityApCost[static_cast<int>(anAbility)];
}

void PlayerSystem::OnTriggerEnter(const TriggerData& someTriggerData)
{
	if (someTriggerData.affectedEntity == myPlayer &&
		(someTriggerData.event == TriggerEvent::LightEnemy_Ability_BasicAttack ||
			someTriggerData.event == TriggerEvent::Boss_FirstAttack ||
			someTriggerData.event == TriggerEvent::Boss_SecondAttack ||
			someTriggerData.event == TriggerEvent::Boss_ThirdAttack ||
			someTriggerData.event == TriggerEvent::CultistEnemy_Ability_Ranged))
	{
		//if (myCanTakeDamage)
		// {
			//myCanTakeDamage = false;
		auto& health = myCoordinator->GetComponent<Health>(someTriggerData.affectedEntity);

		health.TakeDamage(someTriggerData.damage);
		myCombatCooldown->Reset();

		SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_TakeDamage);
		//}

	}
	if (someTriggerData.affectedEntity == myPlayer && (someTriggerData.event == TriggerEvent::SpawnBoss || someTriggerData.event == TriggerEvent::MinibossCutscene))
	{
		if (myMinibossCutscenePlayed == false && someTriggerData.event == TriggerEvent::MinibossCutscene)
		{
			myModel->animator->Play("Idle", true);
			myIsWalking = false;

			Singleton<UIManager>().GetStateStack().PushState(eStateID::Cutscene);
			Singleton<UIManager>().GetStateStack().GetCutscene()->PlayCutscene(eCutscenes::MinibossIntro);
			myMinibossCutscenePlayed = true;
		}
		else if (myFinalBossSpawned == false && someTriggerData.event == TriggerEvent::SpawnBoss)
		{
			myModel->animator->Play("Idle", true);
			myIsWalking = false;

			AudioSystem::GetInstance()->ChangeMusic(AudioClip::Music_Boss);
			Singleton<UIManager>().GetStateStack().PushState(eStateID::Cutscene);
			Singleton<UIManager>().GetStateStack().GetCutscene()->PlayCutscene(eCutscenes::BossIntro);
			myFinalBossSpawned = true;
		}


	}
	else if (myDialogCutscenePlayed == false && someTriggerData.affectedEntity == myPlayer && someTriggerData.event == TriggerEvent::DialogCutscene)
	{
		Singleton<UIManager>().GetStateStack().PushState(eStateID::Cutscene);
		Singleton<UIManager>().GetStateStack().GetCutscene()->PlayCutscene(eCutscenes::NpcDialog);
		myDialogCutscenePlayed = true;
		Singleton<UIManager>().GetStateGameplay()->SetObjectiveIndex(1);

		myModel->animator->Play("Idle", true);
		myIsWalking = false;
		myWalkAudio.Stop();
	}

	// KEEP THIS AS THE LAST EVENT AS IT WILL DESTROY THE ENTIRE LEVEL
	// AND LOAD A NEW, THEREBY INVALIDATING EVERY POINTERS AND REFERENCES
	if (someTriggerData.affectedEntity == myPlayer &&
		(
			someTriggerData.event == TriggerEvent::Level_1 ||
			someTriggerData.event == TriggerEvent::Level_2 ||
			someTriggerData.event == TriggerEvent::Level_3 ||
			someTriggerData.event == TriggerEvent::Level_4 ||
			someTriggerData.event == TriggerEvent::Level_5
			))
	{
		myWalkAudio.Stop();
		switch (someTriggerData.event)
		{
		case TriggerEvent::Level_1:
			Postmaster::GetInstance()->SendMail(eMessage::eStartLevel1);
			break;
		case TriggerEvent::Level_2:
			Postmaster::GetInstance()->SendMail(eMessage::eStartLevel2);
			break;
		case TriggerEvent::Level_3:
			Postmaster::GetInstance()->SendMail(eMessage::eStartLevel3);
			break;
		case TriggerEvent::Level_4:
			if (myMinibossSystem->AreEnemiesDead())
			{
				Postmaster::GetInstance()->SendMail(eMessage::eStartLevel4);
			}
			break;
		case TriggerEvent::Level_5:
			Postmaster::GetInstance()->SendMail(eMessage::eStartLevel5);
			break;
		default:
			break;
		}
	}
}

const Vector3f PlayerSystem::GetCurrentPosition()
{
	return myTransform->GetPosition();
}

int PlayerSystem::GetCurrentXPLevel()
{
	return myExperienceComponent->GetLevel();
}

bool PlayerSystem::IsConeTouchingSphere(Shape& aShapeA, Transform& aTransformA, Transform& aTransformB) const
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

void PlayerSystem::RecieveMessage(eMessage aMsg)
{
	Transform clickEffect;
	switch (aMsg)
	{
	case eMessage::eLevelUp:
		//Play level up animation

		clickEffect.SetPosition(myTransform->GetPosition());
		SE::CVFXManager::GetInstance().PlayVFX("LevelUp", clickEffect);

		myModel->animator->Play("LevelUp", false, "Idle");
		SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Player_LevelUp);

		myHpComponent->SetHealth(myHpComponent->GetMaxHealth());
		myApComponent->ResetAttackPower();

		if (myExperienceComponent->GetLevel() == myExperienceComponent->GetLevelWhatLevelUnlocksAbility(static_cast<int>(Abilities::AOE)))
		{
			SetAbilityStatus(Abilities::AOE, true);
			myAoeUnlocked = true;
			Postmaster::GetInstance()->SendMail(eMessage::eUnlockAOE);
		}
		else if (myExperienceComponent->GetLevel() == myExperienceComponent->GetLevelWhatLevelUnlocksAbility(static_cast<int>(Abilities::TELEPORT)))
		{
			SetAbilityStatus(Abilities::TELEPORT, true);
			myTelportUnlocked = true;
			Postmaster::GetInstance()->SendMail(eMessage::eUnlockTeleport);
		}
		else if (myExperienceComponent->GetLevel() == myExperienceComponent->GetLevelWhatLevelUnlocksAbility(static_cast<int>(Abilities::HOOK)))
		{
			SetAbilityStatus(Abilities::HOOK, true);
			myHookUnlocked = true;
			Postmaster::GetInstance()->SendMail(eMessage::eUnlockHook);
		}
		else
		{
			Postmaster::GetInstance()->SendMail(eMessage::eGeneralLevelUp);
		}
		/*else if (myExperienceComponent->GetLevel() == myExperienceComponent->GetLevelWhatLevelUnlocksAbility(static_cast<int>(Abilities::ULTIMATE)))
		{
			SetAbilityStatus(Abilities::ULTIMATE, true);
			myUltimateUnlocked = true;
			Postmaster::GetInstance()->SendMail(eMessage::eUnlockUltimate);
		}
		*/
		break;
	case eMessage::ePauseGame:

		StopWalking();


	case eMessage::eStartCutscene:

		StopWalking();

		break;

	default:
		break;
	}
}
