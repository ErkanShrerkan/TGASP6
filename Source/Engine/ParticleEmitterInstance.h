#pragma once
#include "ParticleEmitter.h"
#include "Transform.h"
#include <vector>
#include "Stopwatch.h"

namespace SE
{
    class CParticleEmitterInstance
    {
        friend class CParticleRenderer;
        friend class CParticleEmitterFactory;
    private:
        CTransform myTransform;
        Stopwatch myStopwatch;
        std::vector<CParticleEmitter::SParticleVertex> myParticles;
        CParticleEmitter* myEmitter;
        size_t myActiveParticles = 0;
        CParticleEmitter::SParticleSettings mySettings;
        bool myIsSelfOwned = false;

    public:
        void Init(CParticleEmitter* const anEmitter);
        void Update(float aDeltaTime, const Vector3f& aCameraPosition);
        __forceinline const CTransform& GetTransform() const
        {
            return myTransform;
        }
        __forceinline CTransform& GetTransform()
        {
            return myTransform;
        }
        __forceinline bool IsSelfOwned() const
        {
            return myIsSelfOwned;
        }
        void SetAsSelfOwned(bool aIsSelfOwned = true)
        {
            myIsSelfOwned = aIsSelfOwned;
        }

    private:
        void ResetParticle(CParticleEmitter::SParticleVertex* aParticle, const CParticleEmitter::SParticleSettings& someSettings);
    };
}
