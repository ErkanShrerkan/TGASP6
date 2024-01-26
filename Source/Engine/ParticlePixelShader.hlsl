#include "Structs.hlsli"

PixelOutput_Sprite main(GeometryToPixel_Particle input)
{
    PixelOutput_Sprite result;
    float4 textureColor = particleTexture.Sample(defaultSampler, input.myUV);
    result.myColor.rgba = textureColor.rgba * input.myColor.rgba;
    return result;
}
