#include "pch.h"
#include "ParticleEmitterInstance.h"
#include "ParticleEmitter.h"

namespace SE
{
    void CParticleEmitterInstance::Init(CParticleEmitter* const anEmitter)
    {
        myEmitter = anEmitter;

        auto& settings = myEmitter->GetSettings();
        myStopwatch.SetTimeInterval(1.0f / settings.spawnRate);
        myParticles.resize(myEmitter->GetData().numberOfParticles);
    }

    void CParticleEmitterInstance::Update(float aDeltaTime, const Vector3f& aCameraPosition)
    {
        auto& settings = myEmitter->GetSettings();
        
        myStopwatch.Update(aDeltaTime);
        if (myStopwatch.IsOver() && myActiveParticles < myParticles.size())
        {
            ++myActiveParticles;
        }

        for (size_t i = 0; i < myActiveParticles; ++i)
        {
            CParticleEmitter::SParticleVertex& particle = myParticles[i];
            if (particle.lifeTime < 0.0f)
            {
                ResetParticle(&particle, settings);
            }
            particle.lifeTime -= aDeltaTime;
            
            float percentage = 1.0f - (particle.lifeTime / settings.particle.lifeTime);
            particle.offset += particle.velocity * aDeltaTime;
            particle.velocity.y -= particle.gravity * aDeltaTime;
            particle.size.x = Math::Lerp(particle.startSize, particle.endSize, percentage);
            particle.color.alpha = Math::Lerp(particle.startColor.alpha, particle.endColor.alpha, percentage);
            particle.color.rgb = Math::Lerp(particle.startColor.rgb, particle.endColor.rgb, percentage);
            particle.distanceToCamera = (particle.position.xyz - aCameraPosition).LengthSqr();
            particle.position.xyz = myTransform.GetMatrix().GetPosition() + particle.offset.xyz;

            // SpriteSheet
            if (settings.spriteSheet.isAnimated && particle.spriteSheetTime >= settings.spriteSheet.interval)
            {
                particle.spriteSheetTime = 0.0f;
                particle.horizontalOffset += 1.0f;

                // modulo (not really)
                if (particle.horizontalOffset >= settings.spriteSheet.amount)
                    particle.horizontalOffset = 0.0f;
            }
            particle.spriteSheetTime += aDeltaTime;
        }

        // Sort based on distance
        for (size_t i = 0; i < myActiveParticles; ++i)
        for (size_t j = i; j < myActiveParticles; ++j)
        {
            CParticleEmitter::SParticleVertex& firstParticle = myParticles[i];
            CParticleEmitter::SParticleVertex& secondParticle = myParticles[j];

            if (firstParticle.distanceToCamera < secondParticle.distanceToCamera)
            {
                std::swap(firstParticle, secondParticle);
            }
        }
    }

    void CParticleEmitterInstance::ResetParticle(CParticleEmitter::SParticleVertex* aParticle, const CParticleEmitter::SParticleSettings& someSettings)
    {
        auto& particle = *aParticle;

        // Spawn Radius
        const float degree = Math::DegreeToRadian(static_cast<float>(rand() % 360));
        const float radius = Math::Lerp(0.0f, someSettings.spawnRadius, static_cast<float>(rand() % 100) * 0.01f);
        Vector3f offset;
        offset.x = std::sinf(degree) * radius;
        offset.z = std::cosf(degree) * radius;

        // Physics
        particle.offset.xyz = offset;
        particle.position.w = 1.0f;
        particle.velocity.xyz = someSettings.particle.speed.Random() * someSettings.particle.direction.Random();
        particle.velocity.w = 0.0f;
        particle.gravity = someSettings.particle.gravity.Random();

        // Cosmetic
        particle.color = someSettings.particle.startColor.Random();
        particle.startColor = someSettings.particle.startColor.Random();
        particle.endColor = someSettings.particle.endColor.Random();
        particle.size._1 = someSettings.particle.startSize.Random();
        particle.startSize = someSettings.particle.startSize.Random();
        particle.endSize = someSettings.particle.endSize.Random();
        particle.emissiveStrength = someSettings.particle.emissiveStrength.Random();

        // Attribute
        particle.lifeTime = someSettings.particle.lifeTime;
        
        // Spritesheet
        particle.size._2 = someSettings.spriteSheet.size;
        particle.horizontalOffset = 0.0f;
        particle.spriteSheetTime = 0.0f;
        if (someSettings.spriteSheet.startWithRandomIndex)
        {
            int max = static_cast<int>(someSettings.spriteSheet.amount);
            int start = rand() % max;
            particle.horizontalOffset = static_cast<float>(start);
        }
    }
}
