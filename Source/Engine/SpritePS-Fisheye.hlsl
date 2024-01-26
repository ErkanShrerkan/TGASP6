#include "Structs.hlsli"
#include "Functions.hlsli"

PixelOutput_Sprite main(VertexToPixel_Sprite input)
{
    // Fish Lense
    // Taken from https://www.geeks3d.com/20140213/glsl-shader-library-fish-eye-and-dome-and-barrel-distortion-post-processing-filters/
    float aperture = 178.0;
    float apertureHalf = 0.5 * aperture * (PI / 180.0);
    float maxFactor = sin(apertureHalf);
    
    float2 uv;
    float2 xy = 2.0 * (input.myRectUV) - 1.0;
    float d = length(xy);
    if (d < (2.0 - maxFactor))
    {
        d = length(xy * maxFactor);
        float z = sqrt(1.0 - d * d);
        float r = atan2(d, z) / PI;
        float phi = atan2(xy.y, xy.x);
    
        uv.x = r * cos(phi) + .5f;
        uv.y = r * sin(phi) + .5f;
        
        uv.x = Remap(uv.x, 0, 1, mySpriteRect.x, mySpriteRect.z);
        uv.y = Remap(uv.y, 0, 1, mySpriteRect.y, mySpriteRect.w);
    }
    else
    {
        uv = input.myUV.xy;
    }
    
    float4 color = spriteTexture.Sample(defaultSampler, uv);
    
    float mask = maskTexture.Sample(defaultSampler, input.myRectUV).r;
    color.rgb = LinearToGamma(color.rgb) * mask;
    color.a *= mask;
    
    color *= mySpriteColor;
    
    PixelOutput_Sprite output;
    output.myColor.rgba = color;
    return output;
}
