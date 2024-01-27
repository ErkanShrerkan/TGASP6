#include "pch.h"
#include "FullscreenRenderer.h"
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>
#include <fstream>
#include "Engine.h"

#include "ShaderHelper.h"

#include "DebugProfiler.h"

namespace SE
{
    bool CFullscreenRenderer::Init()
    {
        Helper::ShaderHelper::CreateVertexShader(&myVertexShader, "Shaders/FullscreenVS");

        if (!myVertexShader)
            return false;

        std::array<std::string, EShader_Count> shaderPaths;
        shaderPaths[EShader_Copy              ] = "Shaders/FullScreenPS-Copy";
        shaderPaths[EShader_Luminance         ] = "Shaders/FullScreenPS-Luminance";
        shaderPaths[EShader_GaussianHorizontal] = "Shaders/FullScreenPS-GaussianH";
        shaderPaths[EShader_GaussianVertical  ] = "Shaders/FullScreenPS-GaussianV";
        shaderPaths[EShader_Bloom             ] = "Shaders/FullScreenPS-Bloom";
        shaderPaths[EShader_Border            ] = "Shaders/FullscreenPS-Border";
        shaderPaths[EShader_ToCamera          ] = "Shaders/FullscreenPS-ToCamera";
        shaderPaths[EShader_WaterUniversal    ] = "Shaders/FullscreenPS-WaterUniversal";
        shaderPaths[EShader_WaterMerge        ] = "Shaders/FullscreenPS-WaterMerge";
        shaderPaths[EShader_DebugSpheres      ] = "Shaders/FullscreenPS-DebugSpheres";
        shaderPaths[EShader_Fog               ] = "Shaders/FullscreenPS-Fog";
        shaderPaths[EShader_AlphaBlend        ] = "Shaders/FullscreenPS-AlphaBlend";
		shaderPaths[EShader_AdditiveBlend     ] = "Shaders/FullscreenPS-AdditiveBlend";
        shaderPaths[EShader_Outline           ] = "Shaders/FullscreenPS-Outline";
        shaderPaths[EShader_Highlight         ] = "Shaders/FullscreenPS-Highlight";
        for (uint i = 0u; i < EShader_Count; ++i)
        {
            Helper::ShaderHelper::CreatePixelShader(&myPixelShaders[i], shaderPaths[i]);
            if (myPixelShaders[i] == nullptr)
                return false;
        }

        return true;
    }

    void CFullscreenRenderer::Render(const EShader& aMode)
    {
        ID3D11DeviceContext* const& context = CEngine::GetInstance()->GetDXDeviceContext();

        context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->IASetInputLayout(nullptr);
        context->IASetVertexBuffers(0u, 0u, nullptr, nullptr, nullptr);
        context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0u);

        context->VSSetShader(myVertexShader, nullptr, 0u);
        context->PSSetShader(myPixelShaders[aMode], nullptr, 0u);
        context->Draw(3u, 0u);
        Singleton<Debug::CDebugProfiler>().IncrementDrawCallCount();
    }

    CFullscreenRenderer::~CFullscreenRenderer()
    {
        for (auto& shader : myPixelShaders)
        {
            shader->Release();
        }
        myVertexShader->Release();
    }
}
