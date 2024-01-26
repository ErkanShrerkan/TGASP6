#include "UnlitShaderStructs.hlsli"

texture2D instanceTexture : register(t1);
SamplerState defaultSampler : register(s0);

PixelOutput main(VertexToPixel input)
{
    PixelOutput returnValue;
    float4 textureColor = instanceTexture.Sample(defaultSampler, input.myUV.xy).rgba;
    
    //if (input.myUV.x > 0.5)
    //{
    //    textureColor.r += 1.0;
    //    textureColor = saturate(textureColor);
    //}
    
    
    //float contrast = 0.1;
    
    //textureColor.rgb = pow(abs(textureColor.rgb * 2 - 1), 1 / max(contrast, 0.0001)) * sign(textureColor.rgb - 0.5) + 0.5;

    returnValue.myColor = textureColor * input.myColor;
    return returnValue;
}
