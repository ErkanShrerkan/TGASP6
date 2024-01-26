#include "pch.h"
#include "AnimatorSystem.h"
#include "ModelCollection.h"
#include "Coordinator.h"
#include "Engine\Animator.h"

void AnimatorSystem::Update(float aDeltaTime)
{
	for (auto& entity : myEntities)
	{
		myCoordinator->GetComponent<SE::CAnimator*>(entity)->Update(aDeltaTime);
	}
}
