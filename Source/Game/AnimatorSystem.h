#pragma once
#include "System.h"

namespace SE
{
	class CModel;
	class CTransform;
	class CAnimator;
}

class AnimatorSystem : public System<SE::CModel*, SE::CTransform, SE::CAnimator*>
{
public:
	void Update(float aDeltaTime);
};

