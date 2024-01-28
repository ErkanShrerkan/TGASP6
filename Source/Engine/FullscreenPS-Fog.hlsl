#include "Structs.hlsli"
#include "Functions.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen returnValue;
    
    float2 uv = input.myUV;
    float objDepth = depthTexture.Sample(defaultSampler, input.myUV).r;
    float wtrDepth = depthTexture2.Sample(defaultSampler, input.myUV).r;
    float rayLength = 0x7f7fffff;
    float4 worldPosition = float4(0, 0, 0, 1);
    
    objDepth = objDepth == 0 ? 0x7f7fffff : objDepth;
    wtrDepth = wtrDepth == 0 ? 0x7f7fffff : wtrDepth;
    
    if (objDepth < wtrDepth)
    {
        worldPosition.xyz = positionTexture.Sample(defaultSampler, input.myUV).rgb;
        rayLength = length(positionTexture.Sample(defaultSampler, input.myUV).rgb - FBD_cameraPosition.xyz);
    }
    else
    {
        worldPosition.xyz = positionTexture2.Sample(defaultSampler, input.myUV).rgb;
        rayLength = length(positionTexture2.Sample(defaultSampler, input.myUV).rgb - FBD_cameraPosition.xyz);
    }
    
    float depth = min(objDepth, wtrDepth);
    
    float3 color = FullscreenTexture.Sample(defaultSampler, input.myUV).rgb;
    float3 colorOrg = color;
    float fogMult = max((PPBD_fogMult * (PPBD_useHDR ? 2 : 1)) / 100, 0.0001);
    float fogDensity = max(PPBD_fogDensity / 1000, 0);
    float fogOffset = PPBD_fogOffset / 10000;
    float fog = exp(-pow(fogDensity * (depth + fogOffset), fogMult));

    float3 fogColor = PPBD_useHDR ? GammaToLinear(PPBD_fogColor) : PPBD_fogColor;
    
    if (depth == 0x7f7fffff)
    {
        color = fogColor;
    }
    else
    {
        color = lerp(fogColor, color, clamp(fog, 0.0, 1.0));
    }

    returnValue.myColor = float4(color, 1);
    return returnValue;
}

//PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
//{
//    PixelOutput_Fullscreen returnValue;
//    
//    float2 uv = input.myUV;
//    float depth = depthTexture.Sample(defaultSampler, input.myUV).r;
//    float rayLength = 0x7f7fffff;
//    float4 worldPosition = float4(0, 0, 0, 0);
//
//    worldPosition = positionTexture.Sample(defaultSampler, input.myUV);
//
//    if (worldPosition.a < .5)
//    {
//        discard;
//    }
//    
//    rayLength = length(positionTexture.Sample(defaultSampler, input.myUV).rgb - FBD_cameraPosition.xyz);
//    
//    float3 color = FullscreenTexture.Sample(defaultSampler, input.myUV).rgb;
//    float3 colorOrg = color;
//    float fogMult = max(PPBD_fogMult / 10, 0.0001);
//    float fogDensity = max(PPBD_fogDensity / 1000, 0);
//    float fogOffset = PPBD_fogOffset / 10000;
//    float fog = exp(-pow(fogDensity * (depth + fogOffset), fogMult));
//    color.rgb = PPBD_fogColor;
//    returnValue.myColor.rgb = color;
//    returnValue.myColor.a = max(1. - fog, 0.);
//    return returnValue;
//}