#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen returnValue;
    float3 color1 = FullscreenTexture.Sample(defaultSampler, input.myUV).rgb;
    float3 color2 = FullscreenTexture2.Sample(defaultSampler, input.myUV).rgb;
    
    float luminance = dot(color1, float3(0.2126f, 0.7152f, 0.0722f));
    float3 scaledColor = color2 * (1.f - luminance);
    float3 color = color1 + scaledColor;
    
    returnValue.myColor.rgb = color;
    returnValue.myColor.a = 1;
    return returnValue;
}