#include "Structs.hlsli"
#include "Functions.hlsli"

PixelOutput_Sprite main(GeometryToPixel_Particle input)
{
    PixelOutput_Sprite result;
    float4 textureColor = particleTexture.Sample(defaultSampler, input.myUV);
    float4 color = float4(0, 0, 0, 0);
    color.a = textureColor.a * input.myColor.a;
    color.rgb = textureColor.rgb * input.myColor.rgb * color.a;
    color.rgb = PPBD_useHDR ? GammaToLinear(color.rgb) : color.rgb;
    result.myColor = color;
    return result;
}
