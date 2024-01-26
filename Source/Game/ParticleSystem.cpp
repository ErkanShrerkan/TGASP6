#include "pch.h"
#include "ParticleSystem.h"
#include "Coordinator.h"
#include <Engine/Engine.h>
#include <Engine/Scene.h>
#include <Engine/Camera.h>

void ParticleSystem::Update()
{
	for (const Entity& entity : myEntities)
	{
		auto& transform = myCoordinator->GetComponent<Transform>(entity);
		auto& emitter = myCoordinator->GetComponent<ParticleEmitter>(entity);

		emitter.Update(SE::CEngine::GetInstance()->GetDeltaTime(),
			SE::CEngine::GetInstance()->GetActiveScene()->GetMainCamera()->GetPosition());

		if (!emitter.IsSelfOwned())
			emitter.GetTransform().SetTransform(transform.GetMatrix());
	}
}

void ParticleSystem::Render()
{
	for (const Entity& entity : myEntities)
	{
		auto& transform = myCoordinator->GetComponent<Transform>(entity);
		auto& emitter = myCoordinator->GetComponent<ParticleEmitter>(entity);
		if (SE::CEngine::GetInstance()->GetActiveScene()->GetMainCamera()->PassesCulling(transform.GetTransform().GetPosition(), 10000.0f))
		{
			SE::CEngine::GetInstance()->GetActiveScene()->AddInstance(&emitter);
		}
	}
}
