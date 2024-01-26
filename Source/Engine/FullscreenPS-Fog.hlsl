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
    float fogMult = max(PPBD_fogMult / 100, 0.0001);
    float fogDensity = max(PPBD_fogDensity / 1000, 0);
    float fogOffset = PPBD_fogOffset / 10000;
    float fog = exp(-pow(fogDensity * (depth + fogOffset), fogMult));
    
    if (depth == 0x7f7fffff)
    {
        color = PPBD_fogColor;
    }
    else
    {
        color = lerp(PPBD_fogColor, color, fog);
    }
    
    //float nInShadow = 0;
    //float pixelX = (1.f / PPBD_resolution.x);
    //float pixelY = (1.f / PPBD_resolution.y);
    //
    ////ray
    //float3 right = float3(FBD_cameraTransform[0][0], FBD_cameraTransform[1][0], FBD_cameraTransform[2][0]);
    //float3 up = float3(FBD_cameraTransform[0][1], FBD_cameraTransform[1][1], FBD_cameraTransform[2][1]);
    //float3 forward = float3(FBD_cameraTransform[0][2], FBD_cameraTransform[1][2], FBD_cameraTransform[2][2]);
    //float2 scale = float2(1.f / FBD_toProjection[0][0], 1.f / FBD_toProjection[1][1]);
    //float nearPlane = 0.1;
    //float nearWidth = 2 * nearPlane * scale.x;
    //float nearHeight = nearWidth / ((float(PPBD_resolution.x) / float(PPBD_resolution.y)));
    //float3 cameraPos = FBD_cameraPosition.xyz;
    //float3 nearTopLeft =
    //    (forward * nearPlane) -
    //    (right * nearWidth * .5) +
    //    (up * nearHeight * .5);
    //float3 dir = nearTopLeft + (right * uv.x * nearWidth) - (up * uv.y * nearHeight);
    //dir = normalize(dir);
    // 
    //bool hit = false;
    //int iterations = 0;
    //float3 p = cameraPos;
    //float iterationDistance = max(PPBD_godRaySampleDistance / 100, 0);
    //float sampleDepth = 0;
    //float viewDepth = 0;
    //float cameraDepth = 0;
    //const float shadowBias = 0.0001f;
    //float2 projectedCoord;
    //float falloff = PPBD_godRayFalloff / 10;
    //
    //float rand = (rand_1_05(uv) - 1 * 2);
    //p += dir * iterationDistance * rand;
    //
    //[unroll(128)]
    //while (!hit)
    //{
    //    float4 worldToLightView = mul(FBS_toCamera, float4(p, 1));
    //    float4 lightViewToLightProj = mul(FBS_toProjection, worldToLightView);
    //
    //    projectedCoord.x = ((lightViewToLightProj.x / lightViewToLightProj.w) / 2.f + .5f);
    //    projectedCoord.y = ((-lightViewToLightProj.y / lightViewToLightProj.w) / 2.f + .5f);
    //
    //    uv = float2(projectedCoord.x, projectedCoord.y);
    //    
    //    if (uv.x >= -1 && uv.x <= 2 &&
    //        uv.y >= -1 && uv.y <= 2)
    //    {
    //        viewDepth = (lightViewToLightProj.z / lightViewToLightProj.w) - shadowBias;
    //        
    //        //worldToLightView = mul(FBD_toCamera, float4(p, 1));
    //        //lightViewToLightProj = mul(FBD_toProjection, worldToLightView);
    //        //cameraDepth = lightViewToLightProj.z / lightViewToLightProj.w;
    //        float sampleDepth;
    //            
    //        if (uv.x <= 0)
    //        {
    //            if (uv.y < 0)
    //            {
    //                sampleDepth = shadowTexture0.Sample(wrapSampler, uv).r;
    //            }
    //            else if (uv.y < 1)
    //            {
    //                sampleDepth = shadowTexture3.Sample(wrapSampler, uv).r;
    //            }
    //            else if (uv.y <= 2)
    //            {
    //                sampleDepth = shadowTexture6.Sample(wrapSampler, uv).r;
    //            }
    //        }
    //        else if (uv.x < 1)
    //        {
    //            if (uv.y <= 0)
    //            {
    //                sampleDepth = shadowTexture1.Sample(wrapSampler, uv).r;
    //            }
    //            else if (uv.y < 1)
    //            {
    //                sampleDepth = shadowTexture4.Sample(wrapSampler, uv).r;
    //            }
    //            else if (uv.y <= 2)
    //            {
    //                sampleDepth = shadowTexture7.Sample(wrapSampler, uv).r;
    //            }
    //        }
    //        else if (uv.x <= 2)
    //        {
    //            if (uv.y < 0)
    //            {
    //                sampleDepth = shadowTexture2.Sample(wrapSampler, uv).r;
    //            }
    //            else if (uv.y < 1)
    //            {
    //                sampleDepth = shadowTexture5.Sample(wrapSampler, uv).r;
    //            }
    //            else if (uv.y <= 2)
    //            {
    //                sampleDepth = shadowTexture8.Sample(wrapSampler, uv).r;
    //            }
    //        }
    //            
    //        nInShadow += max((viewDepth * falloff - sampleDepth) * (sampleDepth < viewDepth), 0);
    //    
    //        p += dir * iterationDistance;
    //    
    //        hit = length(p - cameraPos) > rayLength;
    //        iterations++;
    //    }
    //}
    //
    //color = lerp(color, color * LBD_myPosition.rgb, saturate(nInShadow / iterations) * (1 - length(PPBD_fogColor - color)));
    returnValue.myColor = float4(color, 1);
    return returnValue;
}