#include "TextStructs.hlsli"

VertexToPixel main(VertexInput input)
{
    VertexToPixel result;
    
    float4 vertexObjectPos = input.myPosition;
    float4 vertexWorldPos = mul(OB_toWorldSpace, vertexObjectPos);
    float4 vertexViewPos = mul(FB_myToCamera, vertexWorldPos);
    float4 vertexProjectionPos = mul(FB_myToProjection, vertexViewPos);
    
    result.myColor = input.myVxColor;
    result.myPosition = vertexProjectionPos;
    result.myUV = input.myUV;
    
    return result;

}