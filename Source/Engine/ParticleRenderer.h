#pragma once
#include <vector>

namespace SE
{
    class CParticleEmitterInstance;
    class CParticleRenderer
    {
    public:

        void Render(std::vector<CParticleEmitterInstance*>& someParticleEmittersInstances);
    };
}
