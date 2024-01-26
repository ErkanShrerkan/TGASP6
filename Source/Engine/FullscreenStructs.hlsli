#include "BRDFHelper.hlsli"

static const float2 myResolution = float2(1600.0, 900.0);
static const float GaussianKernel5[5] =
{
    0.06136, 0.24477, 0.38774, 0.24477, 0.06136
};

float Remap(float aValue, float aLow1, float aHigh1, float aLow2, float aHigh2)
{
    return aLow2 + (aValue - aLow1) * (aHigh2 - aLow2) / (aHigh1 - aLow1);
}

struct VertexInput
{
    uint myIndex : SV_VertexID;
};

struct VertexToPixel
{
    float4 myPosition : SV_POSITION;
    float2 myUV : UV;
};

struct PixelOutput
{
    float4 myColor : SV_Target;
};

cbuffer FrameBuffer : register(b0)
{
    float4x4 toCamera;
    float4x4 toProjection;
    float4 toDirectionalLight;
    float4 directionalLightColor;
    float3 cameraPosition;
    float1 environmentLightMipCount;
}

cbuffer PostProcessingBuffer : register(b1)
{
    int2 depthTextureSize;
    int2 resolution;
}

Texture2D FullscreenTexture : register(t0);
Texture2D FullscreenTexture2 : register(t1);
Texture2D depthTexture : register(t2);
Texture2D normalTexture : register(t3);
SamplerState defaultSampler : register(s0);

float SampleDepth(float2 uv : UV)
{
    float depth = depthTexture.Sample(defaultSampler, uv).r;
    
    float min = toProjection[3][2] + toProjection[2][3] * 2;
    float max = toProjection[2][2] /*- toProjection[2][3] * 2*/;
    
    depth = Remap(depth, 0.983, 1, 0, 1);
    depth = clamp(depth, 0.0f, 1.0f);
    
    float falloff = 20;
    float depthMod = depth - 1.f;
    //if (depthMod >= -.0175f)
    //{
    //    depth = (-(depthMod * depthMod) * 7) + 1;
    //}
    //else
    //{
    //    depth = (exp((depthMod) * falloff) * (1.5f + depthMod));
    //}
    //depth = -((exp(depth * falloff)) * (-depth + 1)) + 1;
    depth = ((exp(depthMod * falloff)) * (depthMod + 1));
    
    depth = clamp(depth, 0.0f, 1.0f);
    //depth = Remap(depth, .5, 1.0f, 0, 1.05);
    //depth *= depth;
    
    return depth;
}

float4 SampleDepthUnAltered(float2 uv : UV)
{
    return depthTexture.Sample(defaultSampler, uv).rgba;
}

float4 SampleScreen(float2 uv : UV)
{
    return FullscreenTexture.Sample(defaultSampler, uv).rgba;
}

float4 SamplePreFX(float2 uv : UV)
{
    return FullscreenTexture.Sample(defaultSampler, uv).rgba;
}

float4 PixelNormal(float2 uv : UV)
{
    return normalTexture.Sample(defaultSampler, uv).rgba;
}