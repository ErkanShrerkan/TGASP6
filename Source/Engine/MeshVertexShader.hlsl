#include "Structs.hlsli"
#include "Functions.hlsli"

VertexToPixel_GBuffer main(VertexInput_GBuffer input)
{
    VertexToPixel_GBuffer returnValue;
    
    float4 vertexObjectPosition = input.myPosition * OBD_scale;
    float4 normal = float4(input.myNormal, 0);
    float4 binormal = float4(input.myBinormal, 0);
    float4 tangent = float4(input.myTangent, 0);

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
        
        float4x4 boneTransform = mul(boneWeights.x, OBD_Bones[boneIDs.x]);
        boneTransform += mul(boneWeights.y, OBD_Bones[boneIDs.y]);
        boneTransform += mul(boneWeights.z, OBD_Bones[boneIDs.z]);
        boneTransform += mul(boneWeights.w, OBD_Bones[boneIDs.w]);

        vertexObjectPosition = mul(boneTransform, input.myPosition);
        normal = mul(boneTransform, normal);
        binormal = mul(boneTransform, binormal);
        tangent = mul(boneTransform, tangent);
    }
    
    float4 vertexWorldPosition = mul(OBD_toWorld, vertexObjectPosition);
    float4 vertexViewPosition = mul(FBD_toCamera, vertexWorldPosition);
    float4 vertexProjectionPosition = mul(FBD_toProjection, vertexViewPosition);
    
    float3x3 toWorldRotation = (float3x3) OBD_toWorld;
    float3 vertexWorldNormal = mul(toWorldRotation, normal);
    float3 vertexWorldTangent = mul(toWorldRotation, tangent);
    float3 vertexWorldBinormal = mul(toWorldRotation, binormal);
    
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
