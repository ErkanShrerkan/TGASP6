#include "Structs.hlsli"
#include "Functions.hlsli"

PixelOutput_Sprite main(VertexToPixel_Sprite input)
{
    float4 color = spriteTexture.Sample(defaultSampler, input.myUV);
    
    float mask = maskTexture.Sample(defaultSampler, input.myRectUV).r;
    color.rgb = mySpriteIsGamma ? LinearToGamma(color.rgb) : color.rgb;
    color.rgb = PPBD_useHDR ? GammaToLinear(color.rgb) : color.rgb;
    color.rgb = color.rgb * mask;
    color.a *= mask;
    
    color *= mySpriteColor;
    
    PixelOutput_Sprite output;
    output.myColor.rgba = color;
    return output;
}
