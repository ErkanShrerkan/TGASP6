#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen result;
    
    float depth = depthTexture2.Sample(defaultSampler, input.myUV).r;
    float sampleDepth = depthTexture.Sample(defaultSampler, input.myUV).r;
    float4 albedo = albedoTexture2.Sample(defaultSampler, input.myUV).rgba;
    float4 color = float4(0, 0, 0, 0);
    
    if (depth > sampleDepth)
    {
        color.rgb = albedo.rgb;
        color.a = .5;
    }
    else if (albedo.a == 0)
    {
        float pixelX = (1.f / PPBD_resolution.x);
        float pixelY = (1.f / PPBD_resolution.y);
        float searchWidth = 1;
    
        [unroll(5)]
        for (float y = -searchWidth; y <= searchWidth; y++)
        {
            [unroll(5)]
            for (float x = -searchWidth; x <= searchWidth; x++)
            {
                float2 uv = float2(input.myUV.x + x * pixelX, input.myUV.y + y * pixelY);
                float4 alpha = albedoTexture2.Sample(defaultSampler, uv).rgba;
                
                if (alpha.a != 0)
                {
                    color = alpha;
                }
            }
        }
    }
    
    result.myColor = color;
    return result;
}