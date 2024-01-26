#include "Structs.hlsli"
#include "Functions.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen returnValue;
    
    float myDepth = ConvertDepthToNonLinear(depthTexture.Sample(defaultSampler, input.myUV).r);
    float3 color = FullscreenTexture.Sample(defaultSampler, input.myUV).rgb;
    float3 colorCpy = color;
    float2 myPos = input.myUV;
    
    float2 texelSize = float2(1.f / PPBD_fullScreenTextureSize.x, 1.f / PPBD_fullScreenTextureSize.y);
    
    int searchWidth = 5 * (1.5 - clamp(myDepth, 0, 1));
    int sideWidth = searchWidth / 2.0f;
    
    float ratio = 0;
    int checks = 0;
    
    [unroll(8)]
    for (int y = -sideWidth; y < sideWidth + 1; y++)
    {
        [unroll(8)]
        for (int x = -sideWidth; x < sideWidth + 1; x++)
        {
            float2 neighbourPos = float2(myPos.x + x * texelSize.x, myPos.y + y * texelSize.y);
            float neighbourDepth = ConvertDepthToNonLinear(depthTexture.Sample(defaultSampler, neighbourPos).r);
            ratio += (myDepth / neighbourDepth);
            checks++;
        }
    }
    
    float borderStrength = 1 - (ratio / checks);
    borderStrength = clamp(borderStrength, 0, 1);
    if (borderStrength <= 0.01f)
    {
        borderStrength = 0;
    }
    color -= borderStrength * 256;
    
    //float shade = PixelNormal(input.myUV).r;
    //float shadeCpy = shade;
    ////shade *= shade;
    //shade += (shade < .02f ? -2 : 2);
    //shade = clamp(shade, 0, 1);
    
    //color = color - (1 - shade);
    
    returnValue.myColor.rgb = color;
    returnValue.myColor.a = 1.f;
    return returnValue;
}