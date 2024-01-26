#include "Structs.hlsli"
#include "Functions.hlsli"

PixelOutput_Sprite main(VertexToPixel_Sprite input)
{
    float4 color = spriteTexture.Sample(defaultSampler, input.myUV);
    
    float mask = maskTexture.Sample(defaultSampler, input.myRectUV).r;
    color.rgb = LinearToGamma(color.rgb) * mask;
    color.a *= mask;
    
    color *= mySpriteColor; 
    
    float epsilon = 0.0001;
    if (mySpriteData + epsilon < 1)
    {
        float2 uv = input.myRectUV;
        float angle = atan2(uv.x * 2.0 - 1.0, uv.y * 2.0 - 1.0);
        float gradient = angle / (PI * 2.0) + 0.5;
        float gradientDeriv = fwidth(gradient) * 1.5;
        float fill = saturate(1 - mySpriteData);
        float barProgress = smoothstep(fill, fill + gradientDeriv, gradient);
        color.rgb = lerp(color.rgb * 0.25f, color.rgb, barProgress);
    }
    
    PixelOutput_Sprite output;
    output.myColor.rgba = color;
    return output;
}
