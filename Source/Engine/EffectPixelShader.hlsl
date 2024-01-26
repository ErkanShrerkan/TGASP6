#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_GBuffer input)
{
    PixelOutput_GBuffer returnValue;
    float2 uv = input.myUV + OBE_uv;
    float4 color = vfxTexture.Sample(wrapSampler, uv).rgba;
    color *= OBE_color;
    color.rgb *= OBE_color.a;
    returnValue.myColor = color;
    return returnValue;
}