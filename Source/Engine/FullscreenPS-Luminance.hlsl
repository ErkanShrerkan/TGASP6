#include "Structs.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen result;
    
    float3 resource = FullscreenTexture.Sample(defaultSampler, input.myUV).rgb;
    // Gain, no Cutoff
    {
        float luminance = dot(resource, float3(0.2126, 0.7152, 0.0722));
        float cutOff = 0.8;
        if (luminance >= cutOff)
        {
            result.myColor.rgb = resource;
        }
        else if (luminance >= cutOff * 0.5)
        {
            float fade = luminance / cutOff;
            fade = pow(fade, 5);
            result.myColor.rgb = resource * fade;
        }
        else
        {
            result.myColor.rgb = 0.0;
        }
    }
    
    result.myColor.a = 1.0;
    return result;
}