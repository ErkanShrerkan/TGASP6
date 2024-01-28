#include "Functions.hlsli"
#include "Structs.hlsli"

struct GBufferOutput
{
    float4 myWorldPosition : SV_TARGET0;
    float4 myAlbedo : SV_TARGET1;
    float4 myNormal : SV_TARGET2;
    float4 myVertexNormal : SV_TARGET3;
    float3 myMaterial : SV_TARGET4;
    float myAmbientOcclusion : SV_TARGET5;
    float myDepth : SV_TARGET6;
};

GBufferOutput main(VertexToPixel_GBuffer input)
{
    if (input.myWorldPosition.y < PPBD_waterLevel)
    {
        discard;
    }
    
    float2 scaledUV = input.myUV * OBD_UVScale;
    float4 albedo = objAlbedoTexture.Sample(defaultSampler, scaledUV);

    if(albedo.a < .5)
    {
        discard;
    }

    albedo.rgb = GammaToLinear(albedo.rgb);
    float3 normal = objNormalTexture.Sample(defaultSampler, scaledUV).wyz;
    float ambientOcclusion = normal.b;
    
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1.f - saturate(normal.x * normal.x + normal.y * normal.y));
    normal = saturate(normal);
    
    float3x3 TBN = float3x3(
        normalize(input.myTangent.xyz),
        normalize(input.myBinormal.xyz),
        normalize(input.myNormal.xyz)
    );
    
    TBN = transpose(TBN);
    
    float3 pixelNormal = normalize(mul(TBN, normal));
    float3 material = objMaterialTexture.Sample(defaultSampler, scaledUV).rgb;
    
    GBufferOutput output;
    
    output.myWorldPosition = input.myWorldPosition;
    output.myAlbedo = float4(albedo.rgb, 1);
    output.myNormal = float4(pixelNormal, 0);
    output.myVertexNormal = float4(input.myNormal.xyz, 1);
    output.myMaterial = material;
    output.myDepth = input.myDepth;
    output.myAmbientOcclusion = ambientOcclusion;
    
    return output;
}