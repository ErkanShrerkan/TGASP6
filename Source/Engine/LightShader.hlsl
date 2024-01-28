#include "Functions.hlsli"
#include "Structs.hlsli"

float3 EvaluateDirectionalLight(float3 albedoColor, float3 specularColor, float3 normal, float roughness, float3 lightColor, float3 lightDir, float3 viewDir)
{
    float NdL = saturate(dot(normal, lightDir));
    float lambert = NdL;
    float NdV = saturate(dot(normal, viewDir));
    float3 h = normalize(lightDir + viewDir);
    float NdH = saturate(dot(normal, h));
    float VdH = saturate(dot(viewDir, h));
    float LdV = saturate(dot(lightDir, viewDir));
    float a = max(0.001f, roughness * roughness);

    float3 cDiff = Diffuse(albedoColor);
    float3 cSpec = Specular(specularColor, h, viewDir, lightDir, a, NdL, NdV, NdH, VdH, LdV);

    return max(lightColor * lambert * (cDiff * (1.0f - cSpec) + cSpec) * PI, 0);
}

float3 EvaluateAmbience(TextureCube lysBurleyCube, float3 vN, float3 org_normal, float3 to_cam, float perceptualRoughness, float metalness, float3 albedo, float ao, float3 dfcol, float3 spccol)
{
    int numMips = GetNumMips(lysBurleyCube);
    const int nrBrdfMips = numMips + 2 * MIP_OFFSET;
    float VdotN = clamp(dot(to_cam, vN), 0.0, 1.0f);
    const float3 vRorg = 2 * vN * VdotN - to_cam;

    float3 vR = GetSpecularDominantDir(vN, vRorg, RoughnessFromPerceptualRoughness(perceptualRoughness));
    float RdotNsat = saturate(dot(vN, vR));

    float l = BurleyToMip(perceptualRoughness, numMips, RdotNsat);

    float3 specRad = lysBurleyCube.SampleLevel(defaultSampler, vR, l + MIP_OFFSET).xyz;
    float3 diffRad = lysBurleyCube.SampleLevel(defaultSampler, vN, (float) (nrBrdfMips - 1) + 2*MIP_OFFSET).xyz;

    float fT = 1.0 - RdotNsat;
    float fT5 = fT * fT;
    fT5 = fT5 * fT5 * fT;

    spccol = lerp(spccol, (float3) 1.0, fT5);

    float fFade = GetReductionInMicrofacets(perceptualRoughness);
    fFade *= EmpiricalSpecularAO(ao, perceptualRoughness);
    fFade *= ApproximateSpecularSelfOcclusion(vR, org_normal);

    float3 ambientdiffuse = ao * dfcol * diffRad;
    float3 ambientspecular = fFade * spccol * specRad;
    return max(ambientdiffuse + ambientspecular, 0);
}

float3 EvaluatePointLight(float3 albedo, float3 specular, float3 normal, float roughness,
    float3 lightColor, float lightIntensity, float lightRange, float3 lightPos, float3 viewDir, float3 pixelPos)
{
    float3 lightDir = lightPos.xyz - pixelPos.xyz;
    float lightDistance = length(lightDir);
    lightDir = normalize(lightDir);
    
    float NdL = saturate(dot(normal, lightDir));
    float lambert = NdL;
    float NdV = saturate(dot(normal, viewDir));
    float3 h = normalize(lightDir + viewDir);
    float NdH = saturate(dot(normal, h));
    float VdH = saturate(dot(viewDir, h));
    float LdV = saturate(dot(lightDir, viewDir));
    float a = max(0.001f, roughness * roughness);

    float3 cDiff = Diffuse(albedo);
    float3 cSpec = Specular(specular, h, viewDir, lightDir, a, NdL, NdV, NdH, VdH, LdV);

    if(PPBD_useHDR)
    {
	    float linAtt = lightDistance / lightRange;
	    linAtt = saturate(1 - linAtt);
	    //lightDistance /= 100.f;
	    //float physicalAttenuation = saturate(1.f / (lightDistance * lightDistance));
	    float attenuation = lambert * linAtt /* physicalAttenuation*/;
        return max(lightColor * lightIntensity * attenuation * ((cDiff * (1.0 - cSpec) + cSpec) * PI), 0);
    }

    lightDistance /= 100.f;
    float linearAttenuation = 1.0 / (lightDistance * lightDistance);
    linearAttenuation = saturate(linearAttenuation);
    float physicalAttenuation = saturate(1.0 / (lightDistance * lightDistance));
    float attenuation = lambert * linearAttenuation /** physicalAttenuation*/;

	return max(lightColor * lightIntensity * attenuation * ((cDiff * (1.0 - cSpec) + cSpec) * PI), 0);
}

float3 EvaluateSpotLight(float3 albedo, float3 specular, float3 normal,
    float roughness, float3 lightColor, float lightIntensity, float lightRange,
    float3 lightPos, float3 lightDir, float outerAngle, float innerAngle, float3 viewDir, float3 pixelPos)
{
    float3 toLight = lightPos.xyz - pixelPos.xyz;
    float lightDistance = length(toLight);
    toLight = normalize(toLight);

    float NdL = saturate(dot(normal, toLight));
    float lambert = NdL;
    float NdV = saturate(dot(normal, viewDir));
    float3 h = normalize(toLight + viewDir);
    float NdH = saturate(dot(normal, h));
    float VdH = saturate(dot(viewDir, h));
    float LdV = saturate(dot(lightDir, viewDir));
    float a = max(0.001f, roughness * roughness);

    float3 cDiff = Diffuse(albedo);
    float3 cSpec = Specular(specular, h, viewDir, lightDir, a, NdL, NdV, NdH, VdH, LdV);

    float cosOuterAngle = cos(outerAngle);
    float cosInnerAngle = cos(innerAngle);
    float3 lightDirection = lightDir;

    float theta = dot(toLight, normalize(-lightDirection));
    float epsilon = cosInnerAngle - cosOuterAngle;
    float intensity = clamp((theta - cosOuterAngle) / epsilon, 0.0f, 1.0f);
    intensity *= intensity;

    if(PPBD_useHDR)
    {
	    float linAtt = lightDistance / lightRange;
	    linAtt = saturate(1 - linAtt);
	    lightDistance /= 100.f;
	    float physicalAttenuation = saturate(1.f / (lightDistance * lightDistance));
	    float attenuation = lambert * linAtt * physicalAttenuation;
	    float finalAttenuation = lambert * intensity * attenuation;
        return max(lightColor * lightIntensity * finalAttenuation * ((cDiff * (1.0 - cSpec) + cSpec) * PI), 0);
    }

    float linearAttenuation = lightDistance / lightRange;
    linearAttenuation = 1.0f / linearAttenuation;
    linearAttenuation = saturate(linearAttenuation);
    float physicalAttenuation = saturate(1.f / (lightDistance * lightDistance));
    float attenuation = lambert * linearAttenuation * physicalAttenuation;
    float finalAttenuation = lambert * intensity * attenuation;

	return max(lightColor * lightIntensity * finalAttenuation * ((cDiff * (1.0 - cSpec) + cSpec) * PI), 0);
}

PixelOutput_Fullscreen EnvironmentLight(VertexToPixel_Fullscreen input)
{
    float4 albedoData = albedoTexture.Sample(defaultSampler, input.myUV);
    if (albedoData.a == 0)
    {
        discard;
    }
    
    PixelOutput_Fullscreen output;
       
    float3 albedo = albedoData.rgb;

    float3 worldPosition = positionTexture.Sample(defaultSampler, input.myUV).rgb;
    float3 normal = normalTexture.Sample(defaultSampler, input.myUV).rgb;
    float3 vertexNormal = vertexNormalTexture.Sample(defaultSampler, input.myUV).rgb;
    float4 material = materialTexture.Sample(defaultSampler, input.myUV);
    
    float metalness = material.r;
    float roughness = material.g;
    float emissiveMask = material.b;
    
    float ao = ambientOcclusionTexture.Sample(defaultSampler, input.myUV).r;
    float depth = depthTexture.Sample(defaultSampler, input.myUV).r;
    
    float3 toEye = normalize(FBD_cameraPosition.xyz - worldPosition.xyz);
    float3 specular = lerp((float3) 0.04, albedo, metalness);
    float3 diffuse = lerp((float3) 0.00, albedo, 1 - metalness);

	float3 directionalLight = EvaluateDirectionalLight(
		diffuse,
		specular,
		normal,
		roughness,
		LBD_myColor.rgb,
		LBD_myDirection.xyz,
		toEye.xyz);
    directionalLight = max(directionalLight * LBD_myColor.a * (PPBD_useHDR ? 2 : 1), 0);
    
    float3 ambience = EvaluateAmbience(
        environmentTexture,
        vertexNormal,
        normal,
        toEye,
        roughness,
        metalness,
        albedo,
        ao,
        diffuse,
        specular
    );
    ambience.rgb *= LBD_myPosition.rgb;
    ambience = max(ambience * LBD_myPosition.a * (PPBD_useHDR ? 2 : 1), 0);
    
    int nInShadow = 0;
    float pixelX = (1.f / 1920.f);
    float pixelY = (1.f / 1080.f);

    float4 worldToLightView = mul(FBS_toCamera, float4(worldPosition, 1));
    float4 lightViewToLightProj = mul(FBS_toProjection, worldToLightView);
    
    float2 projectedCoord;
    projectedCoord.x = ((lightViewToLightProj.x / lightViewToLightProj.w) / 2.f + .5f);
    projectedCoord.y = ((-lightViewToLightProj.y / lightViewToLightProj.w) / 2.f + .5f);
    
    float searchWidth = 5;
    float total = 0;
    [unroll(10)]
    for (int y = -searchWidth; y < searchWidth; y++)
    {
        [unroll(10)]
        for (int x = -searchWidth; x < searchWidth; x++)
        {
            float2 uv = float2(projectedCoord.x + x * pixelX, projectedCoord.y + y * pixelY);
            
            if (uv.x >= -1 && uv.x <= 2 &&
                uv.y >= -1 && uv.y <= 2)
            {
                const float shadowBias = 0.0001f;
                
                float viewDepth = (lightViewToLightProj.z / lightViewToLightProj.w) - shadowBias;
                float sampleDepth;
                
                if (saturate(uv.x) == uv.x && saturate(uv.y) == uv.y)
                {
                    sampleDepth = shadowTexture0.Sample(wrapSampler, uv).r;
                }
                else
                {
                    uv.x = Remap(uv.x, -1, 2, 0, 1);
                    uv.y = Remap(uv.y, -1, 2, 0, 1);
                    sampleDepth = shadowTexture1.Sample(wrapSampler, uv).r;
                }
                if (sampleDepth < viewDepth && sampleDepth > 0)
                {
                    nInShadow++;
                }
            }
        }
    }
    
    float max = (searchWidth * 2) * (searchWidth * 2) * 1.2f;
    
    if (nInShadow > 10)
    {
        directionalLight *= 1.f - ((float) nInShadow / max);
        //ambience *= 1.f - ((float) nInShadow / max);
    }
    
    float3 emissive = albedo * emissiveMask /** 256*/;
    float3 radiance = ambience + directionalLight + emissive;
    
    output.myColor.rgb = PPBD_useHDR ? radiance : LinearToGamma(radiance);
    output.myColor.a = 1.0f;
    return output;
}

PixelOutput_Fullscreen PointLight(VertexToPixel_Fullscreen input)
{
    float4 albedoData = albedoTexture.Sample(defaultSampler, input.myUV);
    if (albedoData.a == 0)
    {
        discard;
    }
    
    PixelOutput_Fullscreen output;
       
    float3 albedo = albedoData.rgb;

    float3 worldPosition = positionTexture.Sample(defaultSampler, input.myUV).rgb;
    float3 normal = normalTexture.Sample(defaultSampler, input.myUV).rgb;
    float3 vertexNormal = vertexNormalTexture.Sample(defaultSampler, input.myUV).rgb;
    float4 material = materialTexture.Sample(defaultSampler, input.myUV);
    
    float metalness = material.r;
    float roughness = material.g;
    float emissiveMask = material.b;
    
    float ao = ambientOcclusionTexture.Sample(defaultSampler, input.myUV).r;
    float depth = depthTexture.Sample(defaultSampler, input.myUV).r;
    
    float3 toEye = normalize(FBD_cameraPosition.xyz - worldPosition.xyz);
    float3 specular = lerp((float3) 0.04, albedo, metalness);
    float3 diffuse = lerp((float3) 0.00, albedo, 1 - metalness);
    
    float3 pointlight = EvaluatePointLight(
        diffuse, specular, normal, roughness,
        LBD_myColor.xyz, LBD_myColor.w,
        LBD_myRange, LBD_myPosition.xyz,
        toEye.xyz, worldPosition.xyz);
    
    output.myColor.rgb = pointlight;
    output.myColor.a = 1;
    
    return output;
}

PixelOutput_Fullscreen SpotLight(VertexToPixel_Fullscreen input)
{
    float4 albedoData = albedoTexture.Sample(defaultSampler, input.myUV);
    if (albedoData.a == 0)
    {
        discard;
    }
    
    PixelOutput_Fullscreen output;
       
    float3 albedo = albedoData.rgb;

    float3 worldPosition = positionTexture.Sample(defaultSampler, input.myUV).rgb;
    float3 normal = normalTexture.Sample(defaultSampler, input.myUV).rgb;
    float3 vertexNormal = vertexNormalTexture.Sample(defaultSampler, input.myUV).rgb;
    float4 material = materialTexture.Sample(defaultSampler, input.myUV);
    
    float metalness = material.r;
    float roughness = material.g;
    float emissiveMask = material.b;
    
    float ao = ambientOcclusionTexture.Sample(defaultSampler, input.myUV).r;
    float depth = depthTexture.Sample(defaultSampler, input.myUV).r;
    
    float3 toEye = normalize(FBD_cameraPosition.xyz - worldPosition.xyz);
    float3 specular = lerp((float3) 0.04, albedo, metalness);
    float3 diffuse = lerp((float3) 0.00, albedo, 1 - metalness);
    
    float3 spotLight = EvaluateSpotLight(
            diffuse, specular, normal, roughness,
            LBD_myColor.xyz, LBD_myColor.w,
            LBD_myRange, LBD_myPosition.xyz,
            LBD_myDirection.xyz, LBD_myOuterAngle,
            LBD_myInnerAngle, toEye.xyz, worldPosition.xyz);
    
    output.myColor.rgb = spotLight;
    output.myColor.a = 1;
    
    return output;
}

PixelOutput_Fullscreen main(VertexToPixel_Fullscreen input)
{
    PixelOutput_Fullscreen output;
    if (LBD_mySign == 0)
    {
        output = EnvironmentLight(input);
    }
    else if (LBD_mySign == 1)
    {
        output = PointLight(input);
    }
    else if (LBD_mySign == 2)
    {
        output = SpotLight(input);
    }
    return output;
}

