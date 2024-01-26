#include "pch.h"
#include "LightSystem.h"
#include <Engine/Scene.h>
#include <Engine/Engine.h>
#include <Engine/Camera.h>
#include <Engine/PointLight.h>
#include "Coordinator.h"

void LightSystem::Render()
{
	auto& scene = SE::CEngine::GetInstance()->GetActiveScene();
	for (auto& entity : myEntities)
	{
		auto& light = myCoordinator->GetComponent<Light>(entity);
		// Assume all light are pointlights
		if (SE::CEngine::GetInstance()->GetActiveScene()->GetMainCamera()->PassesCulling(
			light->myPointLight.GetBundledPosition().xyz, light->myPointLight.GetBundledPosition().w))
		{
			light->Update(SE::CEngine::GetInstance()->GetDeltaTime());
			scene->AddInstance(&light->myPointLight);
		}
	}
}
