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
    float2 scaledUV = input.myUV * OBD_UVScale;
    float4 albedo = objAlbedoTexture.Sample(wrapSampler, scaledUV).rgba;
    albedo.rgb = PPBD_useHDR ? GammaToLinear(albedo.rgb) : albedo.rgb;
    if (albedo.a < PPBD_alphaThreshold / 100)
    {
        discard;
    }
    float3 normal = objNormalTexture.Sample(wrapSampler, scaledUV).wyz;
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
    float3 material = objMaterialTexture.Sample(wrapSampler, scaledUV).rgb;
    
    float4 viewPosition = mul(FBD_toCamera, input.myWorldPosition);
    float4 projectionPosition = mul(FBD_toProjection, viewPosition);
    
    GBufferOutput output;
    output.myWorldPosition = input.myWorldPosition;
    output.myAlbedo = albedo;
    output.myNormal = float4(pixelNormal, 1);
    output.myVertexNormal = float4(input.myNormal.xyz, 1);
    output.myMaterial = material;
    output.myDepth = input.myDepth;
    output.myAmbientOcclusion = ambientOcclusion;
    
    return output;
}