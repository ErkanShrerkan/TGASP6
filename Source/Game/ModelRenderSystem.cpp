#include "pch.h"
#include "ModelRenderSystem.h"
#include "Coordinator.h"

#include <Engine/Engine.h>
#include <Engine/Camera.h>
#include <Engine/Scene.h>
#include <Engine/Animator.h>

void ModelRenderSystem::Render()
{
	auto& engine = *SE::CEngine::GetInstance();
	float dt = engine.GetDeltaTime();
	auto& scene = engine.GetActiveScene();
	for (const Entity& entity : myEntities)
	{
		auto& transform = myCoordinator->GetComponent<Transform>(entity);
		auto position = transform.GetPosition();
		/*if (-1000.f < position.x
			&& position.x < 1000.f
			&& -1000.f < position.z
			&& position.z < 1000.f
		)*/
		{
			auto& collection = myCoordinator->GetComponent<ModelCollection>(entity);
			if (collection.animator &&
				scene->GetMainCamera()->PassesCulling(
					position, collection.model->GetRadius()
			))
			{
				// Add bool that delays updating the animation
				collection.animator->Update(dt);
			}
			scene->AddInstance(&collection, transform.GetTransform(), transform.GetScale());
		}

		// TODO: Cull Models
		
	}
}
