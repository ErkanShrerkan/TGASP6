#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen result;
    
    float2 uv = input.myUV;
    float3 pos = positionTexture2.Sample(defaultSampler, uv).rgb;
    float3 normal = normalTexture2.Sample(defaultSampler, uv).rgb;
    float4 albedo = albedoTexture2.Sample(defaultSampler, uv);
    float depth = depthTexture2.Sample(defaultSampler, uv).r;
    float sampleDepth = depthTexture.Sample(defaultSampler, uv).r;
    float4 color = float4(0, 0, 0, 0);
    
    if (depth <= sampleDepth && depth != 0)
    {
        float d = dot(normalize(normal), normalize(FBD_cameraPosition.xyz - pos));
        color.rgb = albedo.rgb;
        color.a = 1 - (d + 1) * .5f;
    }
   
    result.myColor = color;
    return result;
}