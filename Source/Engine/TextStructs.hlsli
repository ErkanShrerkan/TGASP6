struct VertexInput
{
    float4 myPosition : POSITION;
    float4 myVxColor : COLOR;
    float2 myUV : UV;
};

struct VertexToPixel
{
    float4 myPosition : SV_POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
};

struct PixelOutput
{
    float4 myColor : SV_TARGET;
};

cbuffer FrameBuffer : register(b0)
{
    float4x4 FB_myToCamera;
    float4x4 FB_myToProjection;
    float4 FB_myCameraPosition;
    float FB_myNearPlane;
    float FB_myFarPlane;
    int FB_myRenderMode;
    float garbage;
}

cbuffer ObjectBuffer : register(b1)
{
    float4x4 OB_toWorldSpace;
    
}

Texture2D fontTexture : register(t0);
SamplerState defaultSampler : register(s0);