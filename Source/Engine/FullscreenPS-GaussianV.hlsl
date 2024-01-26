#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen returnValue;
    
    float texelSize = 1.f / (PPBD_resolution.y / 8.f);
    float3 blurColor = 0;
    
    unsigned int kernelSize = 5;
    float start = (((float) (kernelSize) - 1.0f) / 2.0f) * -1.0f;
    for (unsigned int idx = 0; idx < kernelSize; idx++)
    {
        float2 uv = input.myUV + float2(0.0f, texelSize * (start + (float) idx));
        float3 color = FullscreenTexture.Sample(defaultSampler, uv).rgb;
        blurColor += color * GaussianKernel5[idx];
    }
    
    returnValue.myColor.rgb = blurColor;
    returnValue.myColor.a = 1;
    return returnValue;
}