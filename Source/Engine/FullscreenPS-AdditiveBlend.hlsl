#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen result;
    
    float4 color0 = FullscreenTexture.Sample(defaultSampler, input.myUV).rgba;
    float4 color1 = FullscreenTexture2.Sample(defaultSampler, input.myUV).rgba;
    
    result.myColor.rgb = color0 + color1;
    result.myColor.a = 1.0;
    
    return result;
}