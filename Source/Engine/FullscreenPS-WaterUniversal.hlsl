#include "Structs.hlsli"
#include "Functions.hlsli"

bool IntersectPlane(
    const float3 normal,
    const float3 planePoint,
    const float3 rayPoint,
    const float3 rayDir,
    out float d, out float3 p)
{
    d = 0;
    p = 0;
    
    float denom = dot(normalize(normal), normalize(rayDir));
    float epsilon = 1.f / 1000000.f;
    
    if (abs(denom) > epsilon)
    {
        float t = dot(planePoint - rayPoint, normal) / denom;
        p = rayPoint + normalize(rayDir) * t;
        float3 pAlt = rayPoint + rayDir * t;
        d = length(pAlt - rayPoint) * epsilon;
        return (t >= 0);
    }
    
    return false;
}

struct GBufferOutput
{
    float4 myWorldPosition : SV_TARGET0;
    float4 myAlbedo : SV_TARGET1;
    float4 myNormal : SV_TARGET2;
    float4 myVertexNormal : SV_TARGET3;
    float3 myMaterial : SV_TARGET4;
    float myAmbientOcclusion : SV_TARGET5;
    float myDepth : SV_TARGET6;
};

GBufferOutput main(VertexToPixel_Fullscreen input)
{
    float2 uv = input.myUV;
    float3 worldPos = positionTexture.Sample(defaultSampler, uv).rgb;
    float waterHeight = PPBD_waterLevel /* + cos(PPBD_TimePI) * 5*/;
    float dist = waterHeight - worldPos.y; // (-5 - 0 < 0) == true
    
    if (dist < 0)
    {
        discard;
    }
    
    bool cameraAboveWater = FBD_cameraPosition.y > waterHeight;
    float d;
    float depth = depthTexture.Sample(defaultSampler, uv).r;
    float3 p;
    float3 dir;
    float3 color;
    float3 baseColor = FullscreenTexture.Sample(defaultSampler, uv).rgb;
    if (depth == 0)
    {
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
        
        dir = nearTopLeft + (right * uv.x * nearWidth) - (up * uv.y * nearHeight);
        
        if (cameraAboveWater)
        {
            if (!IntersectPlane
            (
                float3(0, 1, 0),
                float3(0, waterHeight, 0),
                cameraPos,
                dir,
                d,
                p
            ))
            {
                discard;
            }
        }
        
        d = 0x7f7fffff;
    }
    else
    {
        if (!IntersectPlane
        (
            float3(0, 1, 0),
            float3(0, waterHeight, 0),
            worldPos,
            FBD_cameraPosition.xyz - worldPos,
            d,
            p
        ))
        {
            discard;
        }
    }
    
    float opticalDepth = 1 - exp(-d * PPBD_depthMult);
    float alphaMod = 1 - exp(-d * PPBD_alphaMult);
    float3 waterColor = lerp(PPBD_shallowWaterColor, PPBD_deepWaterColor, opticalDepth);
    color = lerp(baseColor + waterColor * 0.2f, waterColor, alphaMod); // albedo
        
    GBufferOutput output;
    float3 pn = float3(0, 1, 0); // normal | vertexnormal
    float4 viewPos = mul(FBD_toCamera, float4(p, 1));
    float4 projectionPos = mul(FBD_toProjection, viewPos);
    float3 pd = projectionPos.z / projectionPos.w; // depth
    
    // first normal
    float tileSize = 1600;   // in cm
    float speed = 4;      // in cm
    float2 nuvOffsetScale = normalize(float2(0.05, .005));   // water offset direction
    float2 nuv = float2(float(p.x), float(p.z));        // nuv = normal uv coords
    float2 moveFactor = nuvOffsetScale * PPBD_TimePI * speed;
    nuv += moveFactor;
    nuv /= tileSize;
    float3 normal = normalTexture2.Sample(wrapSampler, nuv).wyz;
    float4 albedo = albedoTexture2.Sample(wrapSampler, nuv).rgba;
    float3 material = materialTexture2.Sample(wrapSampler, nuv).rgb;
    
    // second normal
    tileSize = 512;
    speed = 32;
    nuvOffsetScale = normalize(float2(-.1, -1));
    nuv = float2(float(p.x), float(p.z));
    moveFactor = nuvOffsetScale * PPBD_TimePI * speed;
    nuv += moveFactor;
    nuv /= tileSize;
    float3 normal2 = ambientOcclusionTexture2.Sample(wrapSampler, nuv).wyz;
    
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1.f - saturate(normal.x * normal.x + normal.y * normal.y));
    normal2 = 2.0f * normal2 - 1.0f;
    normal2.z = sqrt(1.f - saturate(normal2.x * normal2.x + normal2.y * normal2.y));
    normal = normalize(lerp(normal, normal2, .25));
    
    if (FBD_cameraPosition.y < waterHeight)
    {
        albedo.rgb = color;
        material = float3(0, 1, 0);
        normal = float3(0, 1, 0);
    }
    
    color = lerp(color, PPBD_waterBorderColor, saturate(1 - ((d * 100) / PPBD_borderThreshhold)));
    
    output.myWorldPosition = float4(p, 1);
    output.myAlbedo = float4(lerp(color, albedo.rgb, material.g), 1);
    output.myNormal = float4(normal.rbg, 0);
    output.myVertexNormal = float4(0, 1, 0, 0);
    output.myMaterial = material;
    //metalness = material.r;
    //roughness = material.g;
    //emissiveMask = material.b;
    output.myDepth = pd;
    output.myAmbientOcclusion = 1;
    return output;
}