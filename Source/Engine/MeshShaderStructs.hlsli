#include "ShaderDefines.h"

struct VertexInput
{
    float4 myPosition : POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
    float3 myNormal : NORMAL;
    float3 myTangent : TANGENT;
    float3 myBinormal : BINORMAL;
};

struct VertexToPixel
{
    float4 myPosition : SV_POSITION;
    float4 myWorldPosition : POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
    float3 myNormal : NORMAL;
    float3 myTangent : TANGENT;
    float3 myBinormal : BINORMAL;
};

struct PixelOutput
{
    float4 myColor : SV_TARGET;
};

cbuffer FrameBuffer : register(b0)
{
    float4x4 toCameraSpace;
    float4x4 toProjectionSpace;
    float4 toDirectionalLight;
    float4 directionalLightColor;
    float3 cameraPosition;
    float1 environmentLightMipCount;
}

cbuffer ObjectBuffer : register(b1)
{
    float4x4 toWorldSpace;
    float4 scale;
    
    uint myAmountOfPointsLights;
    uint myAmountOfSpotLights;
    uint2 garbachoUno;

    struct SPointLightData
    {
        float4 myPosition;
        float4 myColor;
    }
	myPointLights[MAX_POINT_LIGHTS];
    
    struct SSpotLightData
    {
        float4 myPosition;
        float4 myDirection;
        float4 myColor;
        float myInnerAngle;
        float myOuterAngle;
        float2 garbachoDos;
    }
    mySpotLights[MAX_SPOT_LIGHTS];
}
