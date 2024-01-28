#include "pch.h"
#include "AnimatorSystem.h"
#include "ModelCollection.h"
#include "Coordinator.h"
#include "Engine\Animator.h"
#include "UIManager.h"
#include "StateStack.h"

void AnimatorSystem::Update(float aDeltaTime)
{
	if (Singleton<UIManager>().GetStateStack().GetCurrentState().GetStateID() == eStateID::Pause)
	{
		return;
	}

	for (auto& entity : myEntities)
	{
		myCoordinator->GetComponent<SE::CAnimator*>(entity)->Update(aDeltaTime);
	}
}
