#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen result;
    
    float texelSize = 1.0 / (PPBD_resolution.x / 8.0);
    float3 blurColor = 0.0;
    
    uint kernelSize = 5;
    float start = (((float) kernelSize - 1.0) / 2.0) * -1.0;
    for (uint index = 0; index < kernelSize; ++index)
    {
        float2 uv = input.myUV + float2(texelSize * (start + (float) index), 0.0);
        float3 color = FullscreenTexture.Sample(defaultSampler, uv).rgb;
        blurColor += color * GaussianKernel5[index];
    }
    
    result.myColor.rgb = blurColor;
    result.myColor.a = 1.0;
    return result;
}
