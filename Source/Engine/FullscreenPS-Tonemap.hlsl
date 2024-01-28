// Copyright(c) 2022 Bj�rn Ottosson

#include "Structs.hlsli"
#include "Functions.hlsli"

float3 s_curve(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    float3 f = float3(
    clamp((x.x * (a * x.x + b)) / (x.x * (c * x.x + d) + e), 0.0, 1.0),
    clamp((x.y * (a * x.y + b)) / (x.y * (c * x.y + d) + e), 0.0, 1.0),
    clamp((x.z * (a * x.z + b)) / (x.z * (c * x.z + d) + e), 0.0, 1.0));
    return f;
}


float3 tonemap_s_gamut3_cine(float3 c)
{
    // based on Sony's s gamut3 cine
    float3x3 fromSrgb = float3x3(
        +0.6456794776, +0.2591145470, +0.0952059754,
        +0.0875299915, +0.7596995626, +0.1527704459,
        +0.0369574199, +0.1292809048, +0.8337616753);
        
    float3x3 toSrgb = float3x3(
        +1.6269474099, -0.5401385388, -0.0868088707,
        -0.1785155272, +1.4179409274, -0.2394254004,
        +0.0444361150, -0.1959199662, +1.2403560812);
    
    return mul(toSrgb, s_curve(mul(fromSrgb, c)));
}

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen returnValue;

    float3 color = FullscreenTexture.Sample(defaultSampler, input.myUV).rgb;
    color = tonemap_s_gamut3_cine(color);
    //color += mapping;
    //returnValue.myColor.rgb = color;
    //returnValue.myColor.a = 1;
    //
    //float exposure = 2.5;
    //
    //// reinhard tone mapping
    //float3 mapped = float3(1, 1, 1) - exp(mul(-color, exposure));
    ////mapped = color;
    ////color = tonemap_hue_preserving(color);
    ////color = softClipColor(color);
    ////color = tonemap_per_channel(color);
    returnValue.myColor.rgb = color; //pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));
    returnValue.myColor.a = 1;
    return returnValue;
}