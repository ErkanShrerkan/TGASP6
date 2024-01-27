#include "pch.h"
#include <Engine\DX11.h>
#include <Engine\ContentLoader.h>
#include "Collider.h"
#include "EnemySystem.h"
#include "Coordinator.h"
#include "Trigger.h"
#include "Engine\Engine.h"
#include "Engine/AudioEngine.h"
#include "Audio.h"
#include "ModelCollection.h"
#include "Engine/Animator.h"
#include "Engine/NavMeshLoader.h"
#include "Engine/PathFinding.h"
#include <map>
#include "Engine/CDebugDrawer.h"
#include "Expose.h"
#include <random>
#include <thread>
#include <set>
#include "UIManager.h"
#include "Engine/VFXManager.h"
#include "Postmaster.h"
#include "StateStack.h"
#include "ParticleSystem.h"
#include <Engine\CDebugDrawer.h>
#include "Light.h"

std::atomic_bool myIsSwapping = false;

EnemySystem::~EnemySystem()
{
	UNEXPOSE(myBogScytheDamage);
	UNEXPOSE(myKubbLeffeDamage);
	UNEXPOSE(myCultistDamage);
	UNEXPOSE(myChampionDamage);
	UNEXPOSE(myBogScytheMovementSpeed);
	UNEXPOSE(myKubbLeffeMovementSpeed);
	UNEXPOSE(myCultistMovementSpeed);
	UNEXPOSE(myChampionMovementSpeed);
	UNEXPOSE(myBogScytheStopDistance);
	UNEXPOSE(myKubbLeffeStopDistance);
	UNEXPOSE(myCultistStopDistance);
	UNEXPOSE(myChampionStopDistance);
	UNEXPOSE(myBogScytheAttackCooldown);
	UNEXPOSE(myKubbLeffeAttackCooldown);
	UNEXPOSE(myCultistAttackCooldown);
	UNEXPOSE(myChampionAttackCooldown);
	UNEXPOSE(myBogScytheHealth);
	UNEXPOSE(myKubbLeffeHealth);
	UNEXPOSE(myCultistHealth);
	UNEXPOSE(myChampionHealth);
	UNEXPOSE(myCritChance);
	UNEXPOSE(myCritMultiplier);

	Postmaster::GetInstance()->UnSubscribe(this, eMessage::ePauseGame);

	if (myPathFindThread.joinable())
	{
		myPathFindThread.join();
	}
}

void EnemySystem::Init(Coordinator* aCoordinator, TriggerSystem* aTriggerSystem, PlayerSystem* aPlayerSystem, Experience* anExperienceComponent)
{
	myCoordinator = aCoordinator;
	myTriggersystem = aTriggerSystem;
	myPlayerSystem = aPlayerSystem;

	EXPOSE(myBogScytheDamage);
	EXPOSE(myKubbLeffeDamage);
	EXPOSE(myCultistDamage);
	EXPOSE(myChampionDamage);

	EXPOSE(myBogScytheMovementSpeed);
	EXPOSE(myKubbLeffeMovementSpeed);
	EXPOSE(myCultistMovementSpeed);
	EXPOSE(myChampionMovementSpeed);

	EXPOSE(myBogScytheStopDistance);
	EXPOSE(myKubbLeffeStopDistance);
	EXPOSE(myCultistStopDistance);
	EXPOSE(myChampionStopDistance);

	EXPOSE(myBogScytheAttackCooldown);
	EXPOSE(myKubbLeffeAttackCooldown);
	EXPOSE(myCultistAttackCooldown);
	EXPOSE(myChampionAttackCooldown);

	EXPOSE(myBogScytheHealth);
	EXPOSE(myKubbLeffeHealth);
	EXPOSE(myCultistHealth);
	EXPOSE(myChampionHealth);

	myEnemyHealth[0] = myKubbLeffeHealth;
	myEnemyHealth[1] = myBogScytheHealth;
	myEnemyHealth[2] = myCultistHealth;
	myEnemyHealth[3] = myChampionHealth;

	EXPOSE(myCritChance);
	EXPOSE(myCritMultiplier);

	myExperienceComponent = anExperienceComponent;

	Postmaster::GetInstance()->Subscribe(this, eMessage::ePauseGame);
}

void EnemySystem::Thread()
{
	SE::CPathFinding pathFinder;
	SE::SNavTriangle* playerNavTriangle = myPlayerSystem->GetCurrentTriangle();
	if (playerNavTriangle == nullptr)
	{
		return;
	}

	pathFinder.BjornFill(myNavTriangles, playerNavTriangle->myIndex, myNextFloodFillNodes);
	if (myPreviousFloodFillNodes.size() <= 0)
	{
		myPreviousFloodFillNodes = myNextFloodFillNodes;
	}
	myIsSwapping = true;
	myPreviousFloodFillNodes = myNextFloodFillNodes;
	//std::swap(myNextFloodFillNodes, myPreviousFloodFillNodes);
	myIsSwapping = false;


}

void EnemySystem::Update()
{
	for (auto& node : myPreviousFloodFillNodes)
	{
		if (node.second.f == 0)
		{
			CDebugDrawer::GetInstance().DrawSphere(node.first->myCentroid, 60.0f, { 0,1,1,1 });
		}
		else if (node.second.f == 1)
		{
			CDebugDrawer::GetInstance().DrawSphere(node.first->myCentroid, 50.0f, { 1,0,0,1 });
		}
		else if (node.second.f == 2)
		{
			CDebugDrawer::GetInstance().DrawSphere(node.first->myCentroid, 50.0f, { 0,1,0,1 });
		}
		else if (node.second.f == 3)
		{
			CDebugDrawer::GetInstance().DrawSphere(node.first->myCentroid, 50.0f, { 0,0,1,1 });
		}
		else if (node.second.f == 4)
		{
			CDebugDrawer::GetInstance().DrawSphere(node.first->myCentroid, 50.0f, { 1,1,1,1 });
		}
		else if (node.second.f == 5)
		{
			CDebugDrawer::GetInstance().DrawSphere(node.first->myCentroid, 40.0f, { 1,1,1,1 });
		}
		else if (node.second.f == 6)
		{
			CDebugDrawer::GetInstance().DrawSphere(node.first->myCentroid, 30.0f, { 1,1,1,1 });
		}
		else if (node.second.f == 7)
		{
			CDebugDrawer::GetInstance().DrawSphere(node.first->myCentroid, 20.0f, { 1,1,1,1 });
		}
	}

	if (myPlayerSystem->GetPlayerChangedTriangleThisFrame())
	{
		Thread();
		/*if (myPathFindThread.joinable())
		{
			myPathFindThread.join();
		}
		myPathFindThread = std::thread(&EnemySystem::Thread, this);*/
	}

	BoidBehaviour();

	myEnemyDamage[0] = myKubbLeffeDamage;
	myEnemyDamage[1] = myBogScytheDamage;
	myEnemyDamage[2] = myCultistDamage;
	myEnemyDamage[3] = myChampionDamage;

	myEnemyMovementSpeed[0] = myKubbLeffeMovementSpeed;
	myEnemyMovementSpeed[1] = myBogScytheMovementSpeed;
	myEnemyMovementSpeed[2] = myCultistMovementSpeed;
	myEnemyMovementSpeed[3] = myChampionMovementSpeed;

	myEnemyStopDistance[0] = myKubbLeffeStopDistance;
	myEnemyStopDistance[1] = myBogScytheStopDistance;
	myEnemyStopDistance[2] = myCultistStopDistance;
	myEnemyStopDistance[3] = myChampionStopDistance;

	std::set<Entity> myDeadEnemies;

	for (auto& enemy : myEntities)
	{

		auto& leffe = myCoordinator->GetComponent<Enemy>(enemy);
		auto model = myCoordinator->GetComponent<ModelCollection>(enemy);
		auto& transform = myCoordinator->GetComponent<Transform>(enemy);
		auto position = transform.GetPosition();

		if (leffe.myType == Enemy::eType::eCultist)
		{
			Matrix4x4f m = model.animator->GetJointTransform("Staff_prop_Jnt") * transform.GetMatrix();
			float3 pos = m.GetRow(4).xyz - m.GetRow(3).xyz * 69.414f;
			m.SetRow(4, { pos, 1 });
			CDebugDrawer::GetInstance().DrawSphere(m.GetPosition(), 10, { 1, 0, 0, 1 });
			myCoordinator->GetComponent<ParticleEmitter>(enemy).GetTransform().GetMatrix() = m;
			myCoordinator->GetComponent<Light>(enemy)->SetPosition(pos);
		}

		auto& audio = myCoordinator->GetComponent<AudioComponent>(enemy);

		if (!myCoordinator->GetComponent<Health>(enemy).IsEntityDead())
		{
			//Set hp bar

			if (myCoordinator->GetComponent<Health>(enemy).GetHealth() < myCoordinator->GetComponent<Health>(enemy).GetMaxHealth())
			{
				Singleton<UIManager>().GetStateGameplay()->RenderHPBarAt
				(
					position + Vector3f(0.f, 300.f, 0.f),
					myCoordinator->GetComponent<Health>(enemy).GetHealth() / myCoordinator->GetComponent<Health>(enemy).GetMaxHealth()
				);
			}


			//end

			leffe.myFramesLeftBeforePathfind -= 1;
			if (leffe.myFramesLeftBeforePathfind <= 0)
			{
				PathFind(enemy);
				std::random_device dev;
				std::mt19937 rng(dev());
				std::uniform_int_distribution<std::mt19937::result_type> dist6(10, 30);

				leffe.myFramesLeftBeforePathfind = dist6(rng);
			}

			if (leffe.myGoalVectors.size() > 0)
			{
				leffe.myPosToMoveTo = leffe.myGoalVectors[leffe.myGoalVectors.size() - 1];

				if ((leffe.myPosToMoveTo - transform.GetPosition()).LengthSqr() < 100.0f)
				{
					leffe.myGoalVectors.pop_back();
				}
			}
			else
			{
				leffe.myPosToMoveTo = myPlayerSystem->GetCurrentPosition();
			}

			leffe.myFreezeCooldown->Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);

			Vector3f posDif;
			posDif = leffe.myPosToMoveTo - transform.GetPosition();

			if (posDif.LengthSqr() > 0 && leffe.myFreezeCooldown->IsOver() && leffe.myShouldMove && leffe.myIsAggro)
			{
				MoveEnemy(transform, position, leffe, posDif, enemy, leffe.myType);
			}

			if (!leffe.myCanAttack)
			{
				leffe.myAttackCooldown->Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);
			}

			if (leffe.myAttackCooldown->IsOver())
			{
				leffe.myCanAttack = true;
			}

			if (leffe.myIsWalking && leffe.myFreezeCooldown->IsOver())
			{
				if (leffe.myStartedWalking)
				{
					model.animator->Play("Walk", true, "Idle");
					leffe.myStartedWalking = false;
				}
			}
			else
			{
				if (!leffe.myStartedWalking)
				{
					leffe.myStartedWalking = true;
				}
			}

			auto& playerTransform = myCoordinator->GetComponent<Transform>(leffe.target);
			auto playerPosition = playerTransform.GetPosition();
			audio.Update(myCoordinator->GetComponent<Transform>(enemy).GetPosition(), true);

			Vector3f distanceToPlayer = position - playerPosition;
			distanceToPlayer.LengthSqr();

			if (distanceToPlayer.Length() < leffe.myAggroRange && !leffe.myIsAggro)
			{
				leffe.myIsAggro = true;
			}

			if (myPlayerSystem->IsPlayerDead())
			{
				leffe.myIsAggro = false;
			}

			if (leffe.myIsAggro)
			{
				if (distanceToPlayer.Length() > myEnemyStopDistance[static_cast<int>(leffe.myType)])
				{
					if (leffe.myFreezeCooldown->IsOver())
					{
						leffe.myIsWalking = true;
						leffe.myShouldMove = true;
					}

					if (leffe.myType == Enemy::eType::eCultist)
					{
						leffe.myAnticipationCooldown->Reset();
						leffe.myIsAnticipating = false;
					}


					if (!audio.GetIsPlayingOrStarting())
					{
						audio.Start();
					}
				}
				else
				{

					leffe.myShouldMove = false;

					if (!leffe.myStartedWalking)
					{
						model.animator->Play("Idle", true);
						leffe.myStartedWalking = true;
						leffe.myIsWalking = false;
					}

					if (audio.GetIsPlayingOrStarting())
					{
						audio.Stop(false);
					}


					if (leffe.myType != Enemy::eType::eCultist)
					{
						if (leffe.myCanAttack)
						{
							leffe.myCanAttack = false;
							model.animator->Play("Attack", false, "Idle");
							RotateEnemyTowards(transform, position, playerPosition);
							leffe.myAttackCooldown->Reset();
							BasicAttack(transform, leffe.myType);
						}

					}
					else
					{
						if (leffe.myCanAttack && myDeadEnemies.size() == 0)
						{
							RotateEnemyTowards(transform, position, leffe.myAnticipationTransform.GetPosition());
							Transform t;
							t.SetPosition(transform.GetPosition() - 120.f * transform.GetTransform().GetForward());

							leffe.myAnticipationCooldown->Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);

							printf("%f \n", leffe.myAnticipationCooldown->GetTime());

							if (!leffe.myAnticipationCooldown->IsOver())
							{
								if (!leffe.myIsAnticipating)
								{
									leffe.myAnticipationTransform = playerTransform;
									CultistAnticipation(leffe.myAnticipationTransform);
									leffe.myIsAnticipating = true;
								}
							}
							else
							{
								SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Cultist_AttackFrom, position, true);
								leffe.myIsAnticipating = false;
								model.animator->Play("Attack", false, "Idle");
								CultistAttack(leffe.myAnticipationTransform);
								leffe.myIsAnticipating = false;
								leffe.myCanAttack = false;
								leffe.myAnticipationCooldown->Reset();
								leffe.myAttackCooldown->Reset();
							}
						}
					}


				}
			}
		}
		else
		{
			if (!leffe.myHasAlreadyDied)
			{
				Enemy::eType enemyType = myCoordinator->GetComponent<Enemy>(enemy).myType;


				switch (enemyType)
				{
				case Enemy::eType::eKubbLeffe:
					SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Kubb_Leffe_Death, myCoordinator->GetComponent<Transform>(enemy).GetPosition(), true);
					break;
				case Enemy::eType::eBogSchyte:
					SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_BogSchythe_Death, myCoordinator->GetComponent<Transform>(enemy).GetPosition(), true);
					break;
				case Enemy::eType::eCultist:
					SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Cultist_Death, myCoordinator->GetComponent<Transform>(enemy).GetPosition(), true);
					break;
				case Enemy::eType::eChampion:
					SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Champion_Death, myCoordinator->GetComponent<Transform>(enemy).GetPosition(), true);
					break;
				default:
					break;
				}

				PlayerGainXP(leffe);
				audio.Stop(true);
				model.animator->Play("Death", false);
				model.animator->SetFallbackAnimation("");
				model.shaderType = SE::ShaderType::eNone;

				myCoordinator->RemoveComponent<Collider>(enemy);

				leffe.myHasAlreadyDied = true;
			}
			else
			{
				leffe.myDeathSinkTimer->Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);

				if (leffe.myDeathSinkTimer->IsOver())
				{
					leffe.myDeathRemoveCooldown->Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);

					transform.Move({ 0, -40.f * SE::CEngine::GetInstance()->GetDeltaTime(), 0 });

					if (leffe.myDeathRemoveCooldown->IsOver())
					{
						myDeadEnemies.insert(enemy);
					}
				}
			}
		}
	}

	for (auto& enemy : myDeadEnemies)
	{
		myCoordinator->DestroyEntity(enemy);
	}
}

void EnemySystem::PlayerGainXP(Enemy& leffe)
{
	myExperienceComponent->AddXP(static_cast<int>(leffe.myType));
}

void EnemySystem::MoveEnemy(Transform& transform, Vector3f& position, Enemy& leffe, Vector3f& posDif, const Entity& enemy, Enemy::eType anEnemyType)
{
	Vector3f prevPos = transform.GetPosition();
	RotateEnemyTowards(transform, position, leffe.myPosToMoveTo);
	Vector3f movement = transform.GetPosition() + (posDif.GetNormalized() * myEnemyMovementSpeed[static_cast<int>(anEnemyType)] * SE::CEngine::GetInstance()->GetDeltaTime());
	transform.SetPosition(movement);



	bool triangleWasHit = false;

	leffe.myCurrentNavTriangle = FindCurrentTriangle(transform.GetPosition(), triangleWasHit);
	if (triangleWasHit)
	{
		SnapYPositionToNavMesh(enemy);
	}
	else
	{
		transform.SetPosition(prevPos);
		leffe.myCurrentNavTriangle = FindCurrentTriangle(prevPos, triangleWasHit);
		SnapYPositionToNavMesh(enemy);
	}

}



void EnemySystem::PathFind(const Entity& anEntity)
{
	myIsPathfinding = true;
	auto& transform = myCoordinator->GetComponent<Transform>(anEntity);
	auto& leffe = myCoordinator->GetComponent<Enemy>(anEntity);
	if (leffe.myCurrentNavTriangle != nullptr)
	{
		SE::SNavTriangle* playerNavTriangle = myPlayerSystem->GetCurrentTriangle();
		if (playerNavTriangle != nullptr)
		{
			SE::CPathFinding pathfinder;
			if (myPreviousFloodFillNodes.size() > 0 == !myIsSwapping)
			{
				std::vector<int> prevPath = leffe.myPathToPlayer;
				if (myPlayerSystem->GetPlayerChangedTriangleThisFrame())
				{
					leffe.myPathToPlayer = pathfinder.BjornFind(myNavTriangles, myPreviousFloodFillNodes, leffe.myCurrentNavTriangle->myIndex, playerNavTriangle->myIndex, true);
				}

				/*if (prevPath.size() > 0 && (myNavTriangles[leffe.myPathToPlayer[0]]->myCentroid - myNavTriangles[prevPath[0]]->myCentroid).Length() > 1000.0f)
				{
					leffe.myPathToPlayer = prevPath;

				}
				else
				{*/
				if (leffe.myPathToPlayer.size() > 0 && leffe.myPathToPlayer.size() < myNavTriangles.size())
				{
					auto& playerTransform = myCoordinator->GetComponent<Transform>(leffe.target);
					//SE::CPathFinding pathfinder;
					leffe.myGoalVectors = pathfinder.Funnel(myNavTriangles, leffe.myPathToPlayer, transform.GetPosition(), playerTransform.GetPosition());

					leffe.myGoalVectors.pop_back();
					std::reverse(leffe.myGoalVectors.begin(), leffe.myGoalVectors.end());
					leffe.myGoalVectors.pop_back();
				}
				//}
			}


		}

	}
	myIsPathfinding = false;

}


void EnemySystem::SetNavMesh(std::vector<SE::SNavTriangle*> someNavTriangles)
{
	myNavTriangles = someNavTriangles;
}

void EnemySystem::SnapYPositionToNavMesh(const Entity& anEntity)
{
	auto& transform = myCoordinator->GetComponent<Transform>(anEntity);
	auto& leffe = myCoordinator->GetComponent<Enemy>(anEntity);
	if (leffe.myCurrentNavTriangle != nullptr)
	{

		float X = transform.GetPosition().x;
		float Z = transform.GetPosition().z;

		Vector3f vA = leffe.myCurrentNavTriangle->myPoints[0].myPosition;
		Vector3f vB = leffe.myCurrentNavTriangle->myPoints[1].myPosition;
		Vector3f vC = leffe.myCurrentNavTriangle->myPoints[2].myPosition;

		Vector3f v1 = vA - vB;
		Vector3f v2 = vA - vC;


		Vector3f n = v1.Cross(v2);

		float r = n.x; // *x
		float s = n.y; // * y
		float t = n.z; // * z

		float k = r * vA.x + s * vA.y + t * vA.z;
		float Y = (k - X * r - Z * t) / s;
		transform.SetPosition({ transform.GetPosition().x, Y, transform.GetPosition().z });
	}

	/*bool myTriangleWasHit = false;
	for (int i = 0; i < myNavTriangles.size(); i++)
	{
		if (!myTriangleWasHit)
		{
			leffe.myCurrentNavTriangle = myNavTriangles[i];
			myTriangleWasHit = PointIsInsideTriangle(transform.GetPosition().x, transform.GetPosition().z,
				leffe.myCurrentNavTriangle->myPoints[0].myPosition.x, leffe.myCurrentNavTriangle->myPoints[0].myPosition.z,
				leffe.myCurrentNavTriangle->myPoints[1].myPosition.x, leffe.myCurrentNavTriangle->myPoints[1].myPosition.z,
				leffe.myCurrentNavTriangle->myPoints[2].myPosition.x, leffe.myCurrentNavTriangle->myPoints[2].myPosition.z);
		}
	}*/
}


double EnemySystem::CalcTriangleArea(double x1, double y1, double x2, double y2, double x3, double y3)
{
	return abs(x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0;
}

bool EnemySystem::PointIsInsideTriangle(double x, double y, double x1, double y1, double x2, double y2, double x3, double y3)
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
void EnemySystem::OnTriggerEnter(const TriggerData& someTriggerData)
{
	if (myEntities.find(someTriggerData.affectedEntity) == myEntities.end())
		return;

	if (someTriggerData.event == TriggerEvent::Player_Ability_AOE || someTriggerData.event == TriggerEvent::Player_Ability_RmbAttack || someTriggerData.event == TriggerEvent::Player_Ability_Lmb || someTriggerData.event == TriggerEvent::Player_Ability_Ultimate || someTriggerData.event == TriggerEvent::Player_Ability_Teleport)
	{

		ModelCollection& model = myCoordinator->GetComponent<ModelCollection>(someTriggerData.affectedEntity);

		Enemy& enemy = myCoordinator->GetComponent<Enemy>(someTriggerData.affectedEntity);

		auto& health = myCoordinator->GetComponent<Health>(someTriggerData.affectedEntity);

		myCoordinator->GetComponent<Enemy>(someTriggerData.affectedEntity).myIsAggro = true;

		int crit = rand() % 100;

		if (myCritChance < crit)
		{
			if (enemy.myIsWalking)
			{
				enemy.myFreezeCooldown->Reset();
				enemy.myStartedWalking = true;
			}

			model.animator->Play("Hit", false, "Idle", false);
			health.TakeDamage(someTriggerData.damage * myCritMultiplier);
		}
		else
		{
			if (enemy.myIsWalking)
			{
				enemy.myFreezeCooldown->Reset();
				enemy.myStartedWalking = true;
			}

			model.animator->Play("Hit", false, "Idle", false);
			health.TakeDamage(someTriggerData.damage);
		}
		Enemy::eType enemyType = myCoordinator->GetComponent<Enemy>(someTriggerData.affectedEntity).myType;

		SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Kubb_Leffe_Damage, myCoordinator->GetComponent<Transform>(someTriggerData.affectedEntity).GetPosition(), true);

		if (health.IsEntityDead())
		{
			model.shaderType = SE::ShaderType::eNone;
			//myCoordinator->DestroyEntity(someTriggerData.affectedEntity);


			switch (enemyType)
			{
			case Enemy::eType::eKubbLeffe:
				SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Kubb_Leffe_Death, myCoordinator->GetComponent<Transform>(someTriggerData.affectedEntity).GetPosition(), true);
				break;
			case Enemy::eType::eBogSchyte:
				SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_BogSchythe_Death, myCoordinator->GetComponent<Transform>(someTriggerData.affectedEntity).GetPosition(), true);
				break;
			case Enemy::eType::eCultist:
				SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Cultist_Death, myCoordinator->GetComponent<Transform>(someTriggerData.affectedEntity).GetPosition(), true);
				break;
			case Enemy::eType::eChampion:
				SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Champion_Death, myCoordinator->GetComponent<Transform>(someTriggerData.affectedEntity).GetPosition(), true);
				break;
			default:
				break;
			}
		}
	}
	else if (someTriggerData.event == TriggerEvent::Player_Ability_Dammsugare)
	{

		Transform& transform = myCoordinator->GetComponent<Transform>(someTriggerData.affectedEntity);

		myCoordinator->GetComponent<Enemy>(someTriggerData.affectedEntity).myFreezeCooldown->Reset();

		auto& leffe = myCoordinator->GetComponent<Enemy>(someTriggerData.affectedEntity);

		leffe.myCanAttack = false;

		Transform& playerTransform = myCoordinator->GetComponent<Transform>(leffe.target);

		RotateEnemyTowards(transform, transform.GetPosition(), playerTransform.GetPosition());

		float xDif = transform.GetPosition().x - playerTransform.GetPosition().x;
		float zDif = transform.GetPosition().z - playerTransform.GetPosition().z;



		if ((abs(xDif) > 80.f || abs(zDif) > 80.f))
		{
			transform.Move({ 0,0,-50.0f });
		}
	}
}

void EnemySystem::BasicAttack(Transform& aTransform, Enemy::eType anEnemyType)
{
	Trigger myAttackTrigger;
	myAttackTrigger.event = TriggerEvent::LightEnemy_Ability_BasicAttack;
	myAttackTrigger.shape.type = ShapeType::Sphere;
	myAttackTrigger.shape.asSphere.radius = 150.f;
	//Fix correct damage amount, this is the new system, temp damage 100.f
	myAttackTrigger.damage = myEnemyDamage[static_cast<int>(anEnemyType)];
	Transform transform;
	transform.SetPosition(aTransform.GetPosition() - 120.f * aTransform.GetTransform().GetForward());

	myTriggersystem->CreateTrigger(myAttackTrigger, transform);

	switch (anEnemyType)
	{
	case Enemy::eType::eKubbLeffe:
		SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Kubb_Leffe_Attack);
		break;
	case Enemy::eType::eBogSchyte:
		SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_BogSchythe_Attack);
		break;
	case Enemy::eType::eChampion:
		SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Champion_Attack);
		break;
	default:
		break;
	}

}

void EnemySystem::CultistAttack(Transform aTransform)
{
	Trigger myAttackTrigger;
	myAttackTrigger.event = TriggerEvent::CultistEnemy_Ability_Ranged;
	myAttackTrigger.shape.asSphere.radius = 200.f;
	myAttackTrigger.shape.type = ShapeType::Sphere;
	myAttackTrigger.damage = myCultistDamage;
	SE::CAudioEngine::GetInstance()->StartEventOneShot(AudioClip::Enemies_Cultist_Attack);
	myTriggersystem->CreateTrigger(myAttackTrigger, aTransform);
	SE::CVFXManager::GetInstance().PlayVFX("CultistAttack", aTransform);

}

void EnemySystem::CultistAnticipation(Transform aTransform)
{
	SE::CVFXManager::GetInstance().PlayVFX("CultistAnticipation", aTransform);
}

void EnemySystem::BoidBehaviour()
{
	for (auto& enemy : myEntities)
	{
		if (myCoordinator->GetComponent<Health>(enemy).IsEntityDead())
			continue;

		auto& enemyTransform = myCoordinator->GetComponent<Transform>(enemy);
		auto& enemyEnt = myCoordinator->GetComponent<Enemy>(enemy);

		for (auto& target : myEntities)
		{
			if (enemy != target)
			{
				auto& targetTransform = myCoordinator->GetComponent<Transform>(target);
				auto& targetEnemy = myCoordinator->GetComponent<Enemy>(target);

				float xDif = enemyTransform.GetPosition().x - targetTransform.GetPosition().x;
				float zDif = enemyTransform.GetPosition().z - targetTransform.GetPosition().z;
				Vector3f prevPos = enemyTransform.GetPosition();
				bool triangleHit = false;
				if ((abs(xDif) + abs(zDif) < 65.f))
				{
					enemyTransform.SetPosition({ enemyTransform.GetPosition().x + xDif * SE::CEngine::GetInstance()->GetDeltaTime(), enemyTransform.GetPosition().y, enemyTransform.GetPosition().z + zDif * SE::CEngine::GetInstance()->GetDeltaTime() });
					FindCurrentTriangle(enemyTransform.GetPosition(), triangleHit);
					if (!triangleHit)
					{
						enemyTransform.SetPosition(prevPos);
					}

					RotateEnemyTowards(enemyTransform, enemyTransform.GetPosition(), myPlayerSystem->GetCurrentPosition());
				}

				Vector3f dist = enemyTransform.GetPosition() - targetTransform.GetPosition();

				if (dist.Length() < 400.f && !targetEnemy.myIsAggro && enemyEnt.myIsAggro)
				{
					targetEnemy.myIsAggro = true;
				}
			}
		}
	}
}

void EnemySystem::StopWalking()
{

	if (Singleton<UIManager>().GetStateStack().GetCurrentState().GetStateID() == eStateID::Pause)
	{
		for (auto& enemy : myEntities)
		{
			if (!myCoordinator->GetComponent<Health>(enemy).IsEntityDead())
			{
				myCoordinator->GetComponent<Enemy>(enemy).myStartedWalking = true;
				myCoordinator->GetComponent<AudioComponent>(enemy).Stop();
				myCoordinator->GetComponent<ModelCollection>(enemy).animator->Play("Idle", false, "Idle");
			}
		}
	}


}

void EnemySystem::RecieveMessage(eMessage aMsg)
{
	switch (aMsg)
	{
	case eMessage::ePauseGame:
		StopWalking();
		break;
	default:
		break;
	}
}

Entity EnemySystem::SpawnEnemy(Transform& aTransform, Entity aPlayer, Enemy::eType anEnemyType, const float& aAggroRange)
{
	if (anEnemyType == Enemy::eType::eKubbLeffe)
	{
		Entity lightEnemy = myCoordinator->CreateEntity();
		myCoordinator->AddComponent(lightEnemy, aTransform);
		SE::CModel* model{ SE::DX11::Content->GetModelFactory().GetModel("Models/CH_EY_Kubb/CH_EY_Kubb.erc") };
		ModelCollection modelCollect{ model, new SE::CAnimator(model) };
		modelCollect.animator->AddAnimation("Models/CH_EY_Kubb/CH_EY_Kubb_run_AN.myr", "Walk");
		modelCollect.animator->AddAnimation("Models/CH_EY_Kubb/CH_EY_Kubb_Attack_AN.myr", "Attack");
		modelCollect.animator->AddAnimation("Models/CH_EY_Kubb/CH_EY_Kubb_Idle_AN.myr", "Idle");
		modelCollect.animator->AddAnimation("Models/CH_EY_Kubb/CH_EY_Kubb_death_AN.myr", "Death");
		modelCollect.animator->AddAnimation("Models/CH_EY_Kubb/CH_EY_Kubb_dead_AN.myr", "Dead");
		modelCollect.animator->AddAnimation("Models/CH_EY_Kubb/CH_EY_Kubb_getHit_AN.myr", "Hit");
		modelCollect.animator->SetFallbackAnimation("Idle");
		modelCollect.animator->Play("Idle");

		myCoordinator->AddComponent(lightEnemy, modelCollect);

		Collider collider;
		collider.shape.asSphere.radius = 80.0f;
		collider.shape.type = ShapeType::Sphere;
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_AOE);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_RmbAttack);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Dammsugare);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Lmb);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Ultimate);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Teleport);
		myCoordinator->AddComponent(lightEnemy, collider);

		Enemy myKubbLeffe({ aPlayer });
		myKubbLeffe.myAttackCooldown = new Stopwatch(2.5f);
		myKubbLeffe.myFreezeCooldown = new Stopwatch(.75f);
		myKubbLeffe.myDeathSinkTimer = new Stopwatch(7.5f);
		myKubbLeffe.myDeathRemoveCooldown = new Stopwatch(2.f);
		myKubbLeffe.myMovementSpeed = 250.f;
		myKubbLeffe.myType = Enemy::eType::eKubbLeffe;
		myKubbLeffe.myAggroRange = aAggroRange;
		bool triangleWasHit;
		myKubbLeffe.myCurrentNavTriangle = FindCurrentTriangle(myCoordinator->GetComponent<Transform>(lightEnemy).GetPosition(), triangleWasHit);
		myCoordinator->AddComponent(lightEnemy, myKubbLeffe);
		SnapYPositionToNavMesh(lightEnemy);

		Health health;
		health.Init(myEnemyHealth[static_cast<int>(myKubbLeffe.myType)]);
		myCoordinator->AddComponent(lightEnemy, health);

		AudioComponent emptyAudio;
		myCoordinator->AddComponent(lightEnemy, emptyAudio);
		AudioComponent& audio = myCoordinator->GetComponent<AudioComponent>(lightEnemy);
		audio.SetAudioClip(AudioClip::Enemies_Kubb_Leffe_Run);
		return lightEnemy;
	}
	else if (anEnemyType == Enemy::eType::eChampion)
	{
		Entity lightEnemy = myCoordinator->CreateEntity();
		aTransform.SetScale({ 2.0f, 2.0f, 2.0f });
		myCoordinator->AddComponent(lightEnemy, aTransform);
		SE::CModel* model{ SE::DX11::Content->GetModelFactory().GetModel("Models/CH_EY_Kubb/CH_EY_Kubb.erc") };
		ModelCollection modelCollect{ model, new SE::CAnimator(model) };
		modelCollect.animator->AddAnimation("Models/CH_EY_Kubb/CH_EY_Kubb_run_AN.myr", "Walk");
		modelCollect.animator->AddAnimation("Models/CH_EY_Kubb/CH_EY_Kubb_Attack_AN.myr", "Attack");
		modelCollect.animator->AddAnimation("Models/CH_EY_Kubb/CH_EY_Kubb_Idle_AN.myr", "Idle");
		modelCollect.animator->AddAnimation("Models/CH_EY_Kubb/CH_EY_Kubb_death_AN.myr", "Death");
		modelCollect.animator->AddAnimation("Models/CH_EY_Kubb/CH_EY_Kubb_dead_AN.myr", "Dead");
		modelCollect.animator->AddAnimation("Models/CH_EY_Kubb/CH_EY_Kubb_getHit_AN.myr", "Hit");
		modelCollect.animator->SetFallbackAnimation("Idle");
		modelCollect.animator->Play("Idle");

		myCoordinator->AddComponent(lightEnemy, modelCollect);

		Collider collider;
		collider.shape.asSphere.radius = 80.0f;
		collider.shape.type = ShapeType::Sphere;
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_AOE);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_RmbAttack);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Dammsugare);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Lmb);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Ultimate);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Teleport);
		myCoordinator->AddComponent(lightEnemy, collider);

		Enemy myKubbLeffe({ aPlayer });
		myKubbLeffe.myAttackCooldown = new Stopwatch(2.5f);
		myKubbLeffe.myFreezeCooldown = new Stopwatch(.75f);
		myKubbLeffe.myDeathSinkTimer = new Stopwatch(7.5f);
		myKubbLeffe.myDeathRemoveCooldown = new Stopwatch(2.f);
		myKubbLeffe.myMovementSpeed = 250.f;
		myKubbLeffe.myType = Enemy::eType::eChampion;
		myKubbLeffe.myAggroRange = aAggroRange;
		bool triangleWasHit;
		myKubbLeffe.myCurrentNavTriangle = FindCurrentTriangle(myCoordinator->GetComponent<Transform>(lightEnemy).GetPosition(), triangleWasHit);
		myCoordinator->AddComponent(lightEnemy, myKubbLeffe);
		SnapYPositionToNavMesh(lightEnemy);

		Health health;
		health.Init(myEnemyHealth[static_cast<int>(myKubbLeffe.myType)]);
		myCoordinator->AddComponent(lightEnemy, health);

		AudioComponent emptyAudio;
		myCoordinator->AddComponent(lightEnemy, emptyAudio);
		AudioComponent& audio = myCoordinator->GetComponent<AudioComponent>(lightEnemy);
		audio.SetAudioClip(AudioClip::Enemies_Champion_Run);
		return lightEnemy;
	}
	else if (anEnemyType == Enemy::eType::eBogSchyte)
	{
		Entity heavyEnemy = myCoordinator->CreateEntity();
		myCoordinator->AddComponent(heavyEnemy, aTransform);
		SE::CModel* model{ SE::DX11::Content->GetModelFactory().GetModel("Models/CH_EY_BogScythe/CH_EY_BogScythe.erc") };
		ModelCollection modelCollect{ model, new SE::CAnimator(model) };
		modelCollect.animator->AddAnimation("Models/CH_EY_BogScythe/CH_EY_AN_Bogscythe_Walk.myr", "Walk");
		modelCollect.animator->AddAnimation("Models/CH_EY_BogScythe/CH_EY_AN_Bogscythe_Attack.myr", "Attack");
		modelCollect.animator->AddAnimation("Models/CH_EY_BogScythe/CH_EY_AN_Bogscythe_Idle.myr", "Idle");
		modelCollect.animator->AddAnimation("Models/CH_EY_BogScythe/CH_EY_AN_Bogscythe_Death.myr", "Death");
		modelCollect.animator->AddAnimation("Models/CH_EY_BogScythe/CH_EY_AN_Bogscythe_Dead.myr", "Dead");
		modelCollect.animator->AddAnimation("Models/CH_EY_BogScythe/CH_EY_AN_Bogscythe_GetHit.myr", "Hit");
		modelCollect.animator->SetFallbackAnimation("Idle");
		modelCollect.animator->Play("Idle");

		myCoordinator->AddComponent(heavyEnemy, modelCollect);

		Collider collider;
		collider.shape.asSphere.radius = 90.0f;
		collider.shape.type = ShapeType::Sphere;
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_AOE);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_RmbAttack);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Dammsugare);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Lmb);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Ultimate);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Teleport);
		myCoordinator->AddComponent(heavyEnemy, collider);

		Enemy myEnemy({ aPlayer });
		myEnemy.myAttackCooldown = new Stopwatch(2.5f);
		myEnemy.myFreezeCooldown = new Stopwatch(.75f);
		myEnemy.myDeathSinkTimer = new Stopwatch(7.5f);
		myEnemy.myDeathRemoveCooldown = new Stopwatch(2.f);
		myEnemy.myMovementSpeed = 200.f;
		myEnemy.myType = Enemy::eType::eBogSchyte;
		myEnemy.myAggroRange = aAggroRange;
		bool triangleWasHit;
		myEnemy.myCurrentNavTriangle = FindCurrentTriangle(myCoordinator->GetComponent<Transform>(heavyEnemy).GetPosition(), triangleWasHit);

		myCoordinator->AddComponent(heavyEnemy, myEnemy);
		SnapYPositionToNavMesh(heavyEnemy);

		Health health;
		health.Init(myEnemyHealth[static_cast<int>(myEnemy.myType)]);
		myCoordinator->AddComponent(heavyEnemy, health);

		AudioComponent emptyAudio;
		myCoordinator->AddComponent(heavyEnemy, emptyAudio);
		AudioComponent& audio = myCoordinator->GetComponent<AudioComponent>(heavyEnemy);
		audio.SetAudioClip(AudioClip::Enemies_BogSchythe_Walk);
		return heavyEnemy;
	}
	else if (anEnemyType == Enemy::eType::eCultist)
	{
		Entity heavyEnemy = myCoordinator->CreateEntity();
		myCoordinator->AddComponent(heavyEnemy, aTransform);
		SE::CModel* model{ SE::DX11::Content->GetModelFactory().GetModel("Models/CH_EY_Cultist/CH_EY_Cultist.erc") };
		ModelCollection modelCollect{ model, new SE::CAnimator(model) };
		modelCollect.animator->AddAnimation("Models/CH_EY_Cultist/CH_EY_Cultist_Walk_AN.myr", "Walk");
		modelCollect.animator->AddAnimation("Models/CH_EY_Cultist/CH_EY_Cultist_Summon_AN.myr", "Attack");
		modelCollect.animator->AddAnimation("Models/CH_EY_Cultist/CH_EY_Cultist_Idle_AN.myr", "Idle");
		modelCollect.animator->AddAnimation("Models/CH_EY_Cultist/CH_EY_Cultist_Death_AN.myr", "Death");
		modelCollect.animator->AddAnimation("Models/CH_EY_Cultist/CH_EY_Cultist_GetHit_AN.myr", "Hit");
		modelCollect.animator->AddAnimation("Models/CH_EY_Cultist/CH_EY_Cultist_Dead_AN.myr", "Dead");
		modelCollect.animator->SetFallbackAnimation("Idle");
		modelCollect.animator->Play("Idle");

		SE::CParticleEmitter* p = SE::CEngine::GetInstance()->GetContentLoader()->GetParticleFactory().GetParticleEmitter("CultistFlame");
		ParticleEmitter pi;
		pi.Init(p);
		pi.SetAsSelfOwned();
		myCoordinator->AddComponent<ParticleEmitter>(heavyEnemy, pi);

		BaseLight* pointLight = new BaseLight();
		pointLight->SetAsFlicker();
		pointLight->SetColor({ 0.0f, 1.0f, 0.0f });
		pointLight->SetIntensity(7.5f);
		pointLight->SetRange(75.0f);
		myCoordinator->AddComponent(heavyEnemy, pointLight);

		myCoordinator->AddComponent(heavyEnemy, modelCollect);

		Collider collider;
		collider.shape.asSphere.radius = 25.0f;
		collider.shape.type = ShapeType::Sphere;
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_AOE);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_RmbAttack);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Dammsugare);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Lmb);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Ultimate);
		collider.canCollideWithEvents.insert(TriggerEvent::Player_Ability_Teleport);
		myCoordinator->AddComponent(heavyEnemy, collider);

		Enemy myEnemy({ aPlayer });
		myEnemy.myAttackCooldown = new Stopwatch(4.f);
		myEnemy.myFreezeCooldown = new Stopwatch(.75f);
		myEnemy.myDeathSinkTimer = new Stopwatch(7.5f);
		myEnemy.myDeathRemoveCooldown = new Stopwatch(10.f);
		myEnemy.myAnticipationCooldown = new Stopwatch(1.5f);
		myEnemy.myMovementSpeed = 200.f;
		myEnemy.myType = Enemy::eType::eCultist;
		myEnemy.myAggroRange = aAggroRange;
		bool triangleWasHit;
		myEnemy.myCurrentNavTriangle = FindCurrentTriangle(myCoordinator->GetComponent<Transform>(heavyEnemy).GetPosition(), triangleWasHit);

		myCoordinator->AddComponent(heavyEnemy, myEnemy);
		SnapYPositionToNavMesh(heavyEnemy);
		Health health;
		health.Init(myEnemyHealth[static_cast<int>(myEnemy.myType)]);
		myCoordinator->AddComponent(heavyEnemy, health);

		AudioComponent emptyAudio;
		myCoordinator->AddComponent(heavyEnemy, emptyAudio);
		AudioComponent& audio = myCoordinator->GetComponent<AudioComponent>(heavyEnemy);
		audio.SetAudioClip(AudioClip::Enemies_Cultist_Walk);
		return heavyEnemy;
	}
	return INT_MAX;
}


void EnemySystem::RotateEnemyTowards(Transform& anEnemyTransform, const Vector3f& anEnemyPosition, const Vector3f& aTarget)
{
	float lerpRotate = 1.0f;
	float PI = 3.141592653589793238463f;
	Vector2f lookDirection = Vector2f(aTarget.x, aTarget.z) - Vector2f{ anEnemyPosition.x, anEnemyPosition.z };
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
	Vector2f enemyDir;
	enemyDir = { -anEnemyTransform.GetTransform()(3, 1), anEnemyTransform.GetTransform()(3, 3) };
	if (enemyDir.x < 0)
	{
		negative = true;
	}
	float enemyRotateCos = enemyDir.Dot({ 0, 1 }) / (enemyDir.Length() * 1);
	float enemyRotateAngleRad = acos(enemyRotateCos);
	float enemyRotateAngleDeg = enemyRotateAngleRad * 180 / PI;
	enemyRotateAngleDeg;
	if (negative)
	{
		enemyRotateAngleDeg = 360 - enemyRotateAngleDeg;
	}
	float angle = enemyRotateAngleDeg - LookRotateAngleDeg;

	if (angle >= 180.0f)
	{
		angle = 360 - angle;
		angle = -angle;
	}
	else if (angle < -180.0f)
	{
		angle = 360 + angle;
	}
	if (abs(angle) > 90.0f)
	{
		bool lol;
		lol = true;
	}
	anEnemyTransform.Rotate({ 0, angle * lerpRotate, 0 });
}

SE::SNavTriangle* EnemySystem::FindCurrentTriangle(const Vector3f& aPosition, bool& aTriangleWasHit)
{
	SE::SNavTriangle* currentTriangle = nullptr;
	aTriangleWasHit = false;
	for (int i = 0; i < myNavTriangles.size(); i++)
	{
		if (!aTriangleWasHit)
		{
			currentTriangle = myNavTriangles[i];
			aTriangleWasHit = PointIsInsideTriangle(aPosition.x, aPosition.z,
				currentTriangle->myPoints[0].myPosition.x, currentTriangle->myPoints[0].myPosition.z,
				currentTriangle->myPoints[1].myPosition.x, currentTriangle->myPoints[1].myPosition.z,
				currentTriangle->myPoints[2].myPosition.x, currentTriangle->myPoints[2].myPosition.z);
		}
	}
	return currentTriangle;

}