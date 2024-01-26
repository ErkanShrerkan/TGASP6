#include "Structs.hlsli"
#include "Functions.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen returnValue;
    
    float2 uv = input.myUV;
    float3 waterPos = positionTexture2.Sample(defaultSampler, uv).rgb;
    float2 distortion = (normalTexture2.Sample(defaultSampler, uv).rb) * 0.01;
    // water fullscreen normal texture ^
    float waterHeight = PPBD_waterLevel;
    
    float3 objectColor = FullscreenTexture.Sample(defaultSampler, uv).rgb;
    float3 objectPos = positionTexture.Sample(defaultSampler, uv).rgb;
    
    float2 refuv = float2(uv.x, 1 - uv.y);
    float2 refdis = float2(distortion.x, -distortion.y);
    float4 reflectionColor = albedoTexture.Sample(wrapSampler, refuv + refdis).rgba;
    float3 reflectionPos = normalTexture.Sample(wrapSampler, refuv + refdis).rgb;
    float reflectionDist = reflectionPos.y - waterHeight;
    //float3 waterNormal = vertexNormalTexture.Sample(defaultSampler, uv).rgb;
    //float3 material1 = materialTexture.Sample(defaultSampler, uv).rgb;
    //float ao1 = ambientOcclusionTexture.Sample(defaultSampler, uv).r;
    float depth1 = depthTexture.Sample(wrapSampler, uv + distortion).r;
    float4 waterColor = FullscreenTexture2.Sample(wrapSampler, uv + distortion).rgba;
    if (waterColor.a != 1)
    {
        waterColor = FullscreenTexture2.Sample(defaultSampler, uv).rgba;
    }
    //float3 albedo2 = albedoTexture2.Sample(defaultSampler, uv).rgb;
    //float3 normal2 = normalTexture2.Sample(defaultSampler, uv).rgb;
    //float3 vertexNormal2 = vertexNormalTexture2.Sample(defaultSampler, uv).rgb;
    //float3 material2 = materialTexture2.Sample(defaultSampler, uv).rgb;
    //float ao2 = ambientOcclusionTexture2.Sample(defaultSampler, uv).r;
    //float depth2 = depthTexture2.Sample(defaultSampler, uv).r;
    
    float3 color = 0;
    
    if (objectPos.y < waterHeight)
    {
        color = waterColor;
        if (reflectionColor.a > 0)
        {
            float mod = saturate(exp(-(reflectionDist / 100)));
            mod = 1 - exp(-pow(0.01 * (reflectionDist), 2));
            
            int sideWidth = 1;
    
            float ratio = 0;
            int checks = 0;
            
            float2 texelSize = float2(2.f / PPBD_fullScreenTextureSize.x, 2.f / PPBD_fullScreenTextureSize.y);
    
            int blackNeighbours = 0;
            [unroll(3)]
            for (int y = -sideWidth; y < sideWidth + 1; y++)
            {
                [unroll(3)]
                for (int x = -sideWidth; x < sideWidth + 1; x++)
                {
                    float2 neighbourPos = float2
                    (
                        refuv.x + refdis.x + x * texelSize.x,
                        refuv.y + refdis.y + y * texelSize.y
                    );
                    float neighbourAlpha = albedoTexture.Sample(wrapSampler, refuv + refdis).a;
                    if (neighbourAlpha != 1)
                    {
                        blackNeighbours++;
                    }
                }
            }
            
            float3 refCol = reflectionColor.rgb;
            if (blackNeighbours > 2)
            {
                color = waterColor;
            }
            else
            {
                color = lerp(waterColor.rgb, refCol, mod * (1 - materialTexture2.Sample(wrapSampler, uv).g));
            }
        }
    }
    else
    {
        color = objectColor;
    }
    
    returnValue.myColor.rgb = color;
    returnValue.myColor.a = 1;
    return returnValue;
}