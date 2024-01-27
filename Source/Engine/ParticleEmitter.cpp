#include "pch.h"
#include "ParticleEmitter.h"

SE::CParticleEmitter::~CParticleEmitter()
{
	myData.particleVertexBuffer->Release();
}
