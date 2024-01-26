#include "UnlitShaderStructs.hlsli"

cbuffer FrameBuffer : register(b0)
{
    float4x4 toCameraSpace;
    float4x4 toProjectionSpace;
}

cbuffer ObjectBuffer : register(b1)
{
    float4x4 toWorldSpace;
    float4 scale;
}

VertexToPixel main(VertexInput input)
{
    VertexToPixel returnValue;
    
    float4 vertexObjectPosition = input.myPosition * scale;
    float4 vertexWorldPosition = mul(toWorldSpace, vertexObjectPosition);
    float4 vertexViewPosition = mul(toCameraSpace, vertexWorldPosition);
    float4 vertexProjectionPosition = mul(toProjectionSpace, vertexViewPosition);
    
    returnValue.myPosition = vertexProjectionPosition;
    returnValue.myColor = input.myColor;
    returnValue.myUV = input.myUV;
    return returnValue;
}
