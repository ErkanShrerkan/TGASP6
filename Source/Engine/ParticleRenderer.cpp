#include "pch.h"
#include "ParticleRenderer.h"
#include "BufferHelper.h"
#include "ParticleEmitterInstance.h"
#include "Engine.h"

#define SE_PVB_SLOT 0

namespace SE
{
    void CParticleRenderer::Render(std::vector<CParticleEmitterInstance*>& someParticleEmittersInstances)
    {
        for (auto& instance : someParticleEmittersInstances)
        {
            if (instance->myActiveParticles == 0)
                continue;

            auto& emitter = instance->myEmitter;
            auto& data = emitter->GetData();

            Helper::BufferHelper::MapBuffer(
                data.particleVertexBuffer,
                &(instance->myParticles[0]),
                sizeof(CParticleEmitter::SParticleVertex) * instance->myActiveParticles);

            auto& context = CEngine::GetInstance()->GetDXDeviceContext();

            context->IASetPrimitiveTopology(data.primitiveTopology);
            context->IASetInputLayout(data.inputLayout);
            context->IASetVertexBuffers(SE_PVB_SLOT, 1, &data.particleVertexBuffer, &data.stride, &data.offset);

            //context->VSSetConstantBuffers(4, 1, &myObjectBuffer); // <-- identity matrix
            context->VSSetShader(data.vertexShader, nullptr, 0);

            context->GSSetShader(data.geometryShader, nullptr, 0);

            //context->PSSetConstantBuffers(4, 1, &myObjectBuffer); // <-- identity matrix
            context->PSSetShaderResources(29, 1, &data.shaderResourveView);
            context->PSSetShader(data.pixelShader, nullptr, 0);

            //SetBlendState(E_BLENDSTATE_ALPHABLEND);
            //SetBlendState(E_BLENDSTATE_ADDITIVE);
            context->Draw(data.numberOfParticles, 0);
            //SetBlendState(E_BLENDSTATE_DEFAULT);

            context->GSSetShader(nullptr, nullptr, 0);
        }
    }
}

#undef SE_PPS_SLOT
#undef SE_PVB_SLOT
