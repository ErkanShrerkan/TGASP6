#include "MeshShaderStructs.hlsli"
#include "BRDFHelper.hlsli"

TextureCube environtmentCubemapTexture : register(t0);
Texture2D albedoTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D materialTexture : register(t3);
SamplerState defaultSampler : register(s0);

float3 LinearToGamma(float3 aLinearColor)
{
    return pow(abs(aLinearColor), 1.0 / 2.2);
}
float3 GammaToLinear(float3 aGammaColor)
{
    return pow(abs(aGammaColor), 2.2);
}

float3 FromMaterialGet_Albedo(VertexToPixel input)
{
    float3 albedo = albedoTexture.Sample(defaultSampler, input.myUV.xy).rgb;
    albedo = GammaToLinear(albedo);
    return albedo;
}
float3 FromMaterialGet_Normal(VertexToPixel input)
{
    // TGA Channel Pack. We read Alpha as X, Green as Y
    
    float3 normal = float3(normalTexture.Sample(defaultSampler, input.myUV).wy, 0.0);

    normal = 2.0 * normal - 1.0;
    normal.z = saturate(1.0 - sqrt(normal.x * normal.x + normal.y * normal.y));
    
    // Move to Tange Space
    float3x3 TBN = float3x3(
        normalize(input.myTangent),
        normalize(input.myBinormal),
        normalize(input.myNormal)
    );
    normal = mul(normal, TBN);
    normal = normalize(normal);
    return normal;
}
float FromMaterialGet_AmbientOcclusion(VertexToPixel input)
{
    // TGA Channel Pack.
    // AO is stored in Blue channel of the normal map
    float ambientOcclusion = normalTexture.Sample(defaultSampler, input.myUV).b;
    return ambientOcclusion;
}
float FromMaterialGet_Metalness(VertexToPixel input)
{
    // TGA Channel Pack. MRE-S packing
    // Metalness is stored in Red channel of the material texture
    return materialTexture.Sample(defaultSampler, input.myUV.xy).r;
}
float FromMaterialGet_Roughness(VertexToPixel input)
{
    // TGA Channel Pack. MRE-S packing
    // Roughness is stored in Green channel of the material texture
    return materialTexture.Sample(defaultSampler, input.myUV.xy).g;
}
float FromMaterialGet_Emissive(VertexToPixel input)
{
    // TGA Channel Pack. MRE-S packing
    // Emissive is stored in Blue channel of the material texture
    return materialTexture.Sample(defaultSampler, input.myUV.xy).b;
}

float3 EvaluateAmbience(TextureCube aLysBurleyCube, float aMipCount, float3 aViewNormal, float3 aNormal, float3 aToCamera, float aPerceptualRoughness, float aMetalness, float3 anAlbedo, float anAmbientOcclusion, float3 aDiffuseColor, float3 aSpecularColor)
{
    const int brdfMipNumber = (int)(aMipCount - MIP_OFFSET);
    float VdotN = clamp(dot(aToCamera, aViewNormal), 0.0, 1.0);
    const float3 vRorg = 2.0 * aViewNormal * VdotN - aToCamera;
    
    float3 vR = GetSpecularDominantDir(aViewNormal, vRorg, RoughnessFromPerceptualRoughness(aPerceptualRoughness));
    float RdotNsat = saturate(dot(aViewNormal, vR));
    
    float l = BurleyToMip(aPerceptualRoughness, aMipCount, RdotNsat);
    
    float3 specularRad = aLysBurleyCube.SampleLevel(defaultSampler, vR, l).xyz;
    float3 diffuseRad = aLysBurleyCube.SampleLevel(defaultSampler, aViewNormal, (float)(brdfMipNumber - 1)).xyz;
    
    float t = 1.0 - RdotNsat;
    float t5 = t * t * t * t * t;
    aSpecularColor = lerp(aSpecularColor, float3(1.0, 1.0, 1.0), t5);
    
    float fade = GetReductionInMicroFacets(aPerceptualRoughness);
    fade *= EmpiricalSpecularAO(anAmbientOcclusion, aPerceptualRoughness);
    fade *= ApproximateSpecularSelfOcclusion(vR, aNormal);
    
    float3 ambientDiffuse = anAmbientOcclusion * aDiffuseColor * diffuseRad;
    float3 ambientSpecular = fade * aSpecularColor * specularRad;
    return ambientSpecular + ambientSpecular;
}

float3 EvaluateDirectionalLight(float3 aDiffuseColor, float3 aSpecularColor, float3 aNormal, float aPerceptualRoughness, float3 aDirectionalLightColor, float3 aLightDirection, float3 aViewDirection)
{
    float NdL = saturate(dot(aNormal, aLightDirection));
    float lambert = NdL;
    float NdV = saturate(dot(aNormal, aViewDirection));
    float3 h = normalize(aLightDirection + aViewDirection);
    float NdH = saturate(dot(aNormal, h));
    float VdH = saturate(dot(aViewDirection, h));
    float LdV = saturate(dot(aLightDirection, aViewDirection));
    float a = max(0.001, aPerceptualRoughness * aPerceptualRoughness);
    
    float3 diffuseColor = Diffuse(aDiffuseColor);
    float3 specularColor = Specular(aSpecularColor, h, aViewDirection, a, NdL, NdV, NdH);
    
    return saturate(aDirectionalLightColor * lambert * (diffuseColor * (1.0 - specularColor) + specularColor) * PI);
}

float3 EvaluatePointLight(
    float3 aDiffuseColor, float3 aSpecularColor, float3 aNormal, float aPerceptualRoughness,
    float3 aLightColor, float anIntensity, float3 aLightPosition, float aLightRange,
    float3 aViewDirection, float3 aWorldPosition)
{
    float3 lightDirection = aLightPosition - aWorldPosition;
    float lightDistance = length(lightDirection);
    lightDirection = normalize(lightDirection);
    //lightDistance /= 10;
    
    float NdL = saturate(dot(aNormal, lightDirection));
    float lambert = NdL;
    float NdV = saturate(dot(aNormal, aViewDirection));
    float3 h = normalize(lightDirection + aViewDirection);
    float NdH = saturate(dot(aNormal, h));
    float a = max(0.001, aPerceptualRoughness * aPerceptualRoughness);
    
    float3 diffuseColor = Diffuse(aDiffuseColor);
    float3 specularColor = Specular(aSpecularColor, h, aViewDirection, a, NdL, NdV, NdH);
    
    float linearAttenuation = lightDistance / aLightRange;
    linearAttenuation = 1.0f - linearAttenuation;
    linearAttenuation = saturate(linearAttenuation);
    float physicalAttenuation = saturate(1.0f / (lightDistance * lightDistance));
    float attenuation = lambert * linearAttenuation * physicalAttenuation;
    
    anIntensity *= 10000;
    
    return saturate(aLightColor * anIntensity * attenuation * (diffuseColor * (1.0 - specularColor) + specularColor) * PI);
}

float3 EvaluateSpotLight(
    float3 aDiffuseColor, float3 aSpecularColor, float3 aNormal, float aPerceptualRoughness,
    float3 aLightColor, float anIntensity, float3 aLightPosition, float aLightRange,
    float3 aLightDirection, float anOuterAngle, float anInnerAngle,
    float3 aViewDirection, float3 aWorldPosition)
{
    float3 toLight = aLightPosition - aWorldPosition;
    float lightDistance = length(toLight);
    toLight = normalize(toLight);
    
    //if ((lightDistance < 100 && lightDistance > 97.5) || lightDistance < 25)
    //{
    //    return (float3)1.0;
    //}
    
    float NdL = saturate(dot(aNormal, toLight));
    float lambert = NdL;
    float NdV = saturate(dot(aNormal, aViewDirection));
    float3 h = normalize(toLight + aViewDirection);
    float NdH = saturate(dot(aNormal, h));
    float a = max(0.001, aPerceptualRoughness * aPerceptualRoughness);
    
    float3 diffuseColor = Diffuse(aDiffuseColor);
    float3 specularColor = Specular(aSpecularColor, h, aViewDirection, a, NdL, NdV, NdH);
    
    float cosOuterAngle = cos(anOuterAngle);
    float cosInnerAngle = cos(anInnerAngle);
    
    // Determine if pixel is within cone.
    float theta = dot(toLight, normalize(-aLightDirection));
    // And if we're in the inner or other radius.
    float epsilon = cosInnerAngle - cosOuterAngle;
    float intensity = saturate((theta - cosOuterAngle) / epsilon);
    intensity *= intensity;
    
    float linearAttenuation = lightDistance / aLightRange;
    linearAttenuation = 1.0f - linearAttenuation;
    linearAttenuation = saturate(linearAttenuation);
    float physicalAttenuation = saturate(1.0f / (lightDistance * lightDistance));
    float attenuation = lambert * linearAttenuation * physicalAttenuation;
    
    float finalAttenuation = lambert * intensity * attenuation;
    
    anIntensity *= 10000;
    
    return saturate(aLightColor * anIntensity * lambert * finalAttenuation * (diffuseColor * (1.0 - specularColor) + specularColor) * PI);
}

PixelOutput main(VertexToPixel input)
{
	PixelOutput returnValue;
    
    // === PBR ===
	float3 toEye = normalize(cameraPosition.xyz - input.myWorldPosition.xyz);
	float3 albedo = FromMaterialGet_Albedo(input);
	float3 normal = FromMaterialGet_Normal(input);
	float ambientOcclusion = FromMaterialGet_AmbientOcclusion(input);
	float metalness = FromMaterialGet_Metalness(input);
	float perceptualRoughness = FromMaterialGet_Roughness(input);
	float emissiveData = FromMaterialGet_Emissive(input);
    
	float3 specularColor = lerp(0.04, albedo, metalness);
	float3 diffuseColor = lerp(0.00, albedo, 1.0 - metalness);
    
    float3 ambience = EvaluateAmbience(
        environtmentCubemapTexture, environmentLightMipCount,
        normal, input.myNormal.xyz, toEye,
        perceptualRoughness, metalness, albedo,
        ambientOcclusion, diffuseColor, specularColor
    );
    ambience *= directionalLightColor.a;
    
    float3 directionalLight = EvaluateDirectionalLight(
        diffuseColor, specularColor, normal, perceptualRoughness,
        directionalLightColor.xyz, toDirectionalLight.xyz, toEye
    );
    
    float3 pointLights = 0;
    for (uint iP = 0; iP < myAmountOfPointsLights; ++iP)
    {
        pointLights += EvaluatePointLight(
            diffuseColor, specularColor, normal, perceptualRoughness,
            myPointLights[iP].myColor.rgb, myPointLights[iP].myColor.a,
            myPointLights[iP].myPosition.xyz, myPointLights[iP].myPosition.w,
            toEye, input.myWorldPosition.xyz
        );
    }
    
    float3 spotLights = 0;
    for (uint iS = 0; iS < myAmountOfSpotLights; ++iS)
    {
        spotLights += EvaluateSpotLight(
            diffuseColor, specularColor, normal, perceptualRoughness,
            mySpotLights[iS].myColor.rgb, mySpotLights[iS].myColor.a,
            mySpotLights[iS].myPosition.xyz, mySpotLights[iS].myPosition.w,
            mySpotLights[iS].myDirection.xyz, mySpotLights[iS].myOuterAngle,
            mySpotLights[iS].myInnerAngle, toEye, input.myWorldPosition.xyz
        );
    }
    
    float3 emissive = albedo * emissiveData;
    float3 radiance = ambience + directionalLight + emissive + pointLights + spotLights;
    
    returnValue.myColor.rgb = LinearToGamma(radiance);
    returnValue.myColor.a = 1.0;
    // === | ===
    
	return returnValue;
}
