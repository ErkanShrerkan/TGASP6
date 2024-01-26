#include "Structs.hlsli"
#include "Functions.hlsli"

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen result;
    
    float2 uv = input.myUV;
    
    float3 right = float3(FBD_cameraTransform[0][0], FBD_cameraTransform[1][0], FBD_cameraTransform[2][0]);
    float3 up = float3(FBD_cameraTransform[0][1], FBD_cameraTransform[1][1], FBD_cameraTransform[2][1]);
    float3 forward = float3(FBD_cameraTransform[0][2], FBD_cameraTransform[1][2], FBD_cameraTransform[2][2]);
    float2 scale = float2(1.f / FBD_toProjection[0][0], 1.f / FBD_toProjection[1][1]);
        
    float nearPlane = 0.1;
    float nearWidth = 2 * nearPlane * scale.x;
    float nearHeight = nearWidth / ((float(PPBD_resolution.x) / float(PPBD_resolution.y)));
        
    float3 cameraPos = FBD_cameraPosition.xyz;
        
    float3 nearTopLeft =
        (forward * nearPlane) -
        (right * nearWidth * .5) +
        (up * nearHeight * .5);
        
    float3 dir = nearTopLeft + (right * uv.x * nearWidth) - (up * uv.y * nearHeight);
    
    float3 screenColor = FullscreenTexture.Sample(defaultSampler, uv).rgb;
    float3 finalColor = screenColor;
    
    int intersections = 0;
    float dstToObject = distance(cameraPos, positionTexture.Sample(defaultSampler, uv).rgb);
    
    float maxDepthForColor = 50;
    [unroll(128)]
    for (int i = 0; i < DSBD_myNumSpheres; i++)
    {
        if (intersections > 32)
        {
            break;
        }
        
        float3 pos = DSBD_myPositionsAndRadii[i].xyz;
        float radius = DSBD_myPositionsAndRadii[i].w;
        float4 color = DSBD_myColors[i];
        float2 dists = IntersectSphere(pos, radius, cameraPos, dir);
        
        if (!(dists.x > 0 && dists.x < dstToObject))
        {
            continue;
        }

        float depth = dists.y * .25 - dists.x * .25;
        if (depth < 0)
        {
            depth = 0;
        }
        
        finalColor = lerp(finalColor, finalColor + color.xyz, saturate(1 - (depth / maxDepthForColor)));
        
        if (dists.y > dstToObject)
        {
            dists.y = dstToObject;
            depth = dists.y - dists.x;
            if (depth < 0)
            {
                depth = 0;
            }
            finalColor = lerp(finalColor, finalColor + color.xyz, saturate(1 - (depth / maxDepthForColor)));
        }
        else
        {
            dists.x = dists.y;
            dists.y = dstToObject;
            depth = dists.y - dists.x;
            if (depth < 0)
            {
                depth = 0;
            }
            finalColor = lerp(finalColor, finalColor + color.xyz, saturate(1 - (depth / maxDepthForColor)));
        }
        intersections++;
    }
    
    result.myColor = float4(finalColor, 1);
    return result;
}