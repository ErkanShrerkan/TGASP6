#include "Structs.hlsli"
#include "Functions.hlsli"

VertexToPixel_GBuffer main(VertexInput_GBuffer input)
{
    VertexToPixel_GBuffer returnValue;
    
    float4 vertexObjectPosition = float4(input.myPosition.xyz * OBE_scale, 1);
    
    float4 vertexWorldPosition = mul(OBE_toWorld, vertexObjectPosition);
    float4 vertexViewPosition = mul(FBD_toCamera, vertexWorldPosition);
    float4 vertexProjectionPosition = mul(FBD_toProjection, vertexViewPosition);
    
    float3x3 toWorldRotation = (float3x3) OBE_toWorld;
    float3 vertexWorldNormal = mul(toWorldRotation, input.myNormal.xyz);
    float3 vertexWorldTangent = mul(toWorldRotation, input.myTangent.xyz);
    float3 vertexWorldBinormal = mul(toWorldRotation, input.myBinormal.xyz);
    
    returnValue.myPosition = vertexProjectionPosition;
    returnValue.myColor = input.myColor;
    returnValue.myNormal = vertexWorldNormal;
    returnValue.myTangent = vertexWorldTangent;
    returnValue.myBinormal = vertexWorldBinormal;
    
    returnValue.myUV = input.myUV;
    returnValue.myWorldPosition = vertexWorldPosition;
    returnValue.myDepth = vertexProjectionPosition.z > 0 ? vertexProjectionPosition.z / vertexProjectionPosition.w : 0;
    return returnValue;
}
