#pragma once
#include "ForwardRenderer.h"
#include "FullscreenRenderer.h"
#include "FullscreenTexture.h"
#include "DeferredRenderer.h"
#include "GBuffer.h"
#include "Game\TextRenderer.h"
#include "ShadowRenderer.h"
#include "VFXRenderer.h"
#include "ParticleRenderer.h"
#include <Game\Observer.h>

struct CDirectX11Framework;
namespace SE
{
    class CRenderManager : public Observer
    {
    public:
        struct PostProcessingData
        {
            float3 shallowWaterColor;
            float waterLevel;
            float3 deepWaterColor;
            float depthMult;
            float3 waterBorderColor;
            float alphaMult;
            uint2 myTextureSize;
            uint2 resolution;
            float borderThreshhold;
            float TimePI;
            float deltaTime;
            float fogDensity;
            float3 fogColor;
            float fogMult;
            float fogOffset;
            float godRaySampleDistance;
            float godRayFalloff;
            float alphaThreshold;
        };

        enum EBlendState
        {
            E_BLENDSTATE_DISABLE,
            E_BLENDSTATE_ALPHABLEND,
            E_BLENDSTATE_ADDITIVE,

            E_BLENDSTATE_COUNT
        };

        enum EDepthStencilState
        {
            E_DEPTHSTENCILSTATE_DEFAULT,
            E_DEPTHSTENCILSTATE_READONLY,

            E_DEPTHSTENCILSTATE_COUNT
        };

        enum ERasterizerState
        {
            E_RASTERIZERSTATE_DEFAULT,
            E_RASTERIZERSTATE_CULLFRONT,
            E_RASTERIZERSTATE_WIREFRAME,

            E_RASTERIZERSTATE_COUNT
        };

        enum ESamplerState
        {
            E_SAMPLERSTATE_TRILINEAR,
            E_SAMPLERSTATE_TRILINEARWRAP,
            E_SAMPLERSTATE_POINT,

            E_SAMPLERSTATE_COUNT
        };

        bool Init(CDirectX11Framework* aFramework);
        void RenderFrame();
        void RecieveMessage(eMessage aMsg) override;

    private:
        void CreateTextures();

        int myRenderLayer = 0;

        CForwardRenderer myForwardRenderer;
        CDeferredRenderer myDeferredRenderer;
        CFullscreenRenderer myFullscreenRenderer;
        TextRenderer myTextRenderer;
        CShadowRenderer myShadowRenderer;
        CVFXRenderer myVFXRenderer;
        CParticleRenderer myParticleRenderer;

        CGBuffer myGBuffer;
        CGBuffer myWaterBuffer;
        CGBuffer myReflectionBuffer;
        CGBuffer myModelEffectBuffer;

        ID3D11ShaderResourceView* myWaterNormal;
        ID3D11ShaderResourceView* myWaterNormal2;
        ID3D11ShaderResourceView* myWaterAlbedo;
        ID3D11ShaderResourceView* myWaterMaterial;

        CFullscreenTexture myBackBuffer;
        CFullscreenTexture myScaledBackBuffer;
        CFullscreenTexture myDeferredTexture;
        CFullscreenTexture myIntermediateDepth;
        CFullscreenTexture myIntermediateTexture;
        CFullscreenTexture myReflectionTexture;
        CFullscreenTexture myMergedWaterTexture;
        CFullscreenTexture myFullscreenCopy;
        CFullscreenTexture myEffectTexture;
        CFullscreenTexture myModelEffectTextures[2];

        CFullscreenTexture myHalfSizeTexture;
        CFullscreenTexture myLuminanceTexture;
        CFullscreenTexture myQuarterSizeTexture;
        CFullscreenTexture myBlurTexture1;
        CFullscreenTexture myBlurTexture2;

        std::array<ID3D11BlendState*, E_BLENDSTATE_COUNT> myBlendStates;
        std::array<ID3D11DepthStencilState*, E_DEPTHSTENCILSTATE_COUNT> myDepthStencilStates;
        std::array<ID3D11RasterizerState*, E_RASTERIZERSTATE_COUNT> myRasterizerStates;
        std::array<ID3D11SamplerState*, E_SAMPLERSTATE_COUNT> mySamplerStates;

        PostProcessingData myPostProcessingData;

        ID3D11Buffer* myPostProcessingBuffer = nullptr;

        bool myRenderShadows = true;

        void SetBlendState(EBlendState aBlendState);
        void SetDepthStencilState(EDepthStencilState aStencilState);
        void SetRasterizerState(ERasterizerState aRasterizerState);
        void SetSamplerState(ESamplerState aSamplerState, int aSlot = 0);
    public:
        PostProcessingData& GetPPD() { return myPostProcessingData; }
    };
}
