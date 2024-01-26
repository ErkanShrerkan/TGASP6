#include "Structs.hlsli"
#include "Functions.hlsli"

VertexToPixel_GBuffer main(VertexInput_GBuffer input)
{
    VertexToPixel_GBuffer returnValue;
    
    float4 vertexObjectPosition = input.myPosition * OBD_scale;
    
    if (OBD_HasBones)
    {
        float4 boneWeights = input.myBoneWeights;
        uint4 boneIDs = input.myBoneIDs;
        
        float4 skinnedPos = float4(0, 0, 0, 0);
        skinnedPos += boneWeights.x * mul(OBD_Bones[boneIDs.x], input.myPosition);
        skinnedPos += boneWeights.y * mul(OBD_Bones[boneIDs.y], input.myPosition);
        skinnedPos += boneWeights.z * mul(OBD_Bones[boneIDs.z], input.myPosition);
        skinnedPos += boneWeights.w * mul(OBD_Bones[boneIDs.w], input.myPosition);
        vertexObjectPosition = skinnedPos * OBD_scale;
    }
    
    float4 vertexWorldPosition = mul(OBD_toWorld, vertexObjectPosition);
    float4 vertexViewPosition = mul(FBD_toCamera, vertexWorldPosition);
    float4 vertexProjectionPosition = mul(FBD_toProjection, vertexViewPosition);
    
    float3x3 toWorldRotation = (float3x3) OBD_toWorld;
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
