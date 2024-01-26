#pragma once
#include "System.h"
#include <Engine/ParticleEmitterInstance.h>
#include "Transform.h"

typedef SE::CParticleEmitterInstance ParticleEmitter;
class ParticleSystem : public System<ParticleEmitter, Transform>
{
public:
    void Update();
    void Render();
};
