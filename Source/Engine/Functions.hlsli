float BiasFunction(float x, float bias)
{
    float k = pow(1 - bias, 3);
    return (x * k) / (x * k - x + 1);
}

float2 IntersectSphere(
    const float3 sphereCenter,
    const float sphereRadius,
    const float3 rayPoint,
    const float3 rayDir)
{
    float3 oc = rayPoint - sphereCenter;
    float a = dot(rayDir, rayDir);
    float b = 2.0 * dot(oc, rayDir);
    float c = dot(oc, oc) - (sphereRadius * sphereRadius);
    float d = b * b - 4 * a * c;
    
    if (d > 0)
    {
        float s = sqrt(d);
        float dstToSphereNear = max(0, (-b - s) / (2 * a));
        float dstToSphereFar = (-b + s) / (2 * a);
        
        if (dstToSphereFar >= 0)
        {
            return float2(dstToSphereNear * .1, dstToSphereFar * .1);
        }
    }
    else if (d == 0)
    {
        float s = sqrt(d);
        float dstToSphereFar = (-b + s) / (2 * a);
        
        if (dstToSphereFar >= 0)
        {
            return float2(-1, dstToSphereFar * .1);
        }
    }
    
    return float2(-1, -1);
}

float rand_1_05(in float2 uv)
{
    float2 noise = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
    return abs(noise.x + noise.y) * 0.5;
}

float2 rand_2_10(in float2 uv)
{
    float noiseX = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
    float noiseY = sqrt(1 - noiseX * noiseX);
    return float2(noiseX, noiseY);
}

float2 rand_2_0004(in float2 uv)
{
    float noiseX = (frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453));
    float noiseY = (frac(sin(dot(uv, float2(12.9898, 78.233) * 2.0)) * 43758.5453));
    return float2(noiseX, noiseY) * 0.004;
}

float Remap(float aValue, float aLow1, float aHigh1, float aLow2, float aHigh2)
{
    return aLow2 + (aValue - aLow1) * (aHigh2 - aLow2) / (aHigh1 - aLow1);
}

float ConvertDepthToNonLinear(float aDepth)
{
    float depth = Remap(aDepth, 0.1f, 10000, 0, 1);
    //return (exp((-depth) * 50) * (depth - 1)) + 1;
    return (exp((depth - 1) * -2) * ((depth - 1) + 1));
}

float3 LinearToGamma(float3 aLinearColor)
{
    return pow(abs(aLinearColor), 1.0 / 2.2);
}

float3 GammaToLinear(float3 aGammaColor)
{
    return pow(abs(aGammaColor), 2.2);
}

#define FLOAT_EPSILON 1.192092896e-07
#define MIP_OFFSET 3
// ^^^ Skansen has 11 mipmaps and TGA said to use miplevel 8 in task 7

float3 GetSpecularDominantDir(float3 aNormal, float3 anR, float aRealRoughness)
{
    float invertedRealRoughness = saturate(1.0 - aRealRoughness);
    float lerpFactor = invertedRealRoughness * (sqrt(invertedRealRoughness) + aRealRoughness);
    return lerp(aNormal, anR, lerpFactor);
}

float RoughnessFromPerceptualRoughness(float aPerceptualRoughness)
{
    return aPerceptualRoughness * aPerceptualRoughness;
}

float SpecularPowerFromFromPerceptualRoughness(float aPerceptualRoughness)
{
    float roughness = RoughnessFromPerceptualRoughness(aPerceptualRoughness);
    return (2.0 / max(FLOAT_EPSILON, roughness * roughness)) - 2.0;
}

float PerceptualRoughnessFromRoughness(float aRoughness)
{
    return sqrt(max(0.0, aRoughness));
}

float PerceptualRoughnessFromSpecularPower(float aSpecularPower)
{
    float roughness = sqrt(2.0 / (aSpecularPower + 2.0));
    return PerceptualRoughnessFromRoughness(roughness);
}

float BurleyToMip(float aPerceptualRoughness, int someMipLevels, float aNDotR)
{
    float specularPower = SpecularPowerFromFromPerceptualRoughness(aPerceptualRoughness);
    specularPower /= (4.0 * max(aNDotR, FLOAT_EPSILON));
    float scale = PerceptualRoughnessFromSpecularPower(specularPower);
    return scale * (someMipLevels - 1 - MIP_OFFSET);
}

float GetReductionInMicroFacets(float aPerceptualRoughness)
{
    float roughness = RoughnessFromPerceptualRoughness(aPerceptualRoughness);
    return 1.0 / (roughness * roughness + 1.0);
}

float Bias(float aValue, float aBias)
{
    return (aBias > 0.0) ? pow(abs(aValue), log(aBias) / log(0.5)) : 0.0;
}

float Gain(float aValue, float aGain)
{
    return 0.5 * ((aValue < 0.5) ? Bias(2.0 * aValue, 1.0 - aGain) : (2.0 - Bias(2.0 - 2.0 * aValue, 1.0 - aGain)));
}

float EmpiricalSpecularAO(float anAmbientOcclusion, float aPerceptualRoughness)
{
    float smooth = 1.0 - aPerceptualRoughness;
    float specularAmbientOcclusion = Gain(anAmbientOcclusion, 0.5 + max(0.0, smooth * 0.04));
    return min(1.0, specularAmbientOcclusion + lerp(0.0, 0.5, smooth * smooth * smooth * smooth));
}

float ApproximateSpecularSelfOcclusion(float3 aVR, float3 aNormalizedVertexNormal)
{
    const float fadeParameter = 1.3;
    float rimMask = clamp(1.0 + fadeParameter * dot(aVR, aNormalizedVertexNormal), 0.0, 1.0);
    rimMask *= rimMask;

    return rimMask;
}

// Diffuse Helper

static float PI = 3.14;
float3 Diffuse(float3 anAlbedo)
{
    return anAlbedo / PI;
}

float NormalDistributionGGX(float anA, float aNdH)
{
    float a2 = anA * anA;
    float NdH2 = aNdH * aNdH;

    float denominator = NdH2 * (a2 - 1.0) + 1.0;
    denominator *= denominator;
    denominator *= PI;

    return a2 / denominator;
}

float SpecularD(float anA, float aNdH)
{
    return NormalDistributionGGX(anA, aNdH);
}

float GeometricSmithSchlickGGX(float anA, float aNdV, float aNdL)
{
    float k = anA * 0.5;
    float gV = aNdV / (aNdV * (1.0 - k) + k);
    float gL = aNdL / (aNdL * (1.0 - k) + k);

    return gV * gL;
}

float SpecularG(float anA, float aNdV, float aNdL)
{
    return GeometricSmithSchlickGGX(anA, aNdV, aNdL);
}

float3 FresnelSchlick(float3 aSpecularColor, float3 anH, float3 aViewDirection)
{
    return (aSpecularColor + (1.0 - aSpecularColor) * pow(1.0 - saturate(dot(aViewDirection, anH)), 5.0));
}

float3 SpecularF(float3 aSpecularColor, float3 aViewDirection, float3 anH)
{
    return FresnelSchlick(aSpecularColor, anH, aViewDirection);
}

float3 Specular(float3 aSpecularColor, float3 anH, float3 aViewDirection, float anA,
				float aNdL, float aNdV, float aNdH)
{
    return ((SpecularD(anA, aNdH) * SpecularG(anA, aNdV, aNdL)) * SpecularF(aSpecularColor, aViewDirection, anH)) / (4.0 * aNdL * aNdV + 0.0001);
}

float bias(float value, float b)
{
    return (b > 0.0) ? pow(abs(value), log(b) / log(0.5)) : 0.0;
}

float gain(float value, float g)
{
    return 0.5 * ((value < 0.5) ? bias(2.0 * value, 1.0 - g) : (2.0 - bias(2.0 - 2.0 * value, 1.0 - g)));
}

int GetNumMips(TextureCube cubeTex)
{
    int iWidth = 0, iHeight = 0, numMips = 0;
    cubeTex.GetDimensions(0, iWidth, iHeight, numMips);
    return numMips;
}

float GetReductionInMicrofacets(float perceptualRoughness)
{
    float roughness = RoughnessFromPerceptualRoughness(perceptualRoughness);

    return 1.0 / (roughness * roughness + 1.0);
}

float Geometric_Smith_Schlick_GGX(float a, float NdV, float NdL)
{
    float k = a * 0.5f;
    float GV = NdV / (NdV * (1 - k) + k);
    float GL = NdL / (NdL * (1 - k) + k);

    return GV * GL;
}

float Specular_G(float a, float NdV, float NdL, float NdH, float VdH, float LdV)
{
    return Geometric_Smith_Schlick_GGX(a, NdV, NdL);
}

float3 Fresnel_Schlick(float3 specularColor, float3 h, float3 v)
{
    return (specularColor + (1.0f - specularColor) * pow((1.0f - saturate(dot(v, h))), 5));
}

float3 Specular_F(float3 specularColor, float3 h, float3 v)
{
    return Fresnel_Schlick(specularColor, h, v);
}

float NormalDistribution_GGX(float a, float NdH)
{
    float a2 = a * a;
    float NdH2 = NdH * NdH;

    float denominator = NdH2 * (a2 - 1.0f) + 1.0f;

    denominator *= denominator;
    denominator *= PI;

    return a2 / denominator;
}

float Specular_D(float a, float NdH)
{
    return NormalDistribution_GGX(a, NdH);
}

float3 Specular(float3 specularColor, float3 h, float3 v, float3 l, float a, float NdL, float NdV, float NdH, float VdH, float LdV)
{
    return ((Specular_D(a, NdH) * Specular_G(a, NdV, NdL, NdH, VdH, LdV)) * Specular_F(specularColor, v, h)) / (4.0f * NdL * NdV + 0.0001f);
}

//float3 EvaluateAmbience(TextureCube aLysBurleyCube, float aMipCount, float3 aViewNormal, float3 aNormal, float3 aToCamera, float aPerceptualRoughness, float aMetalness, float3 anAlbedo, float anAmbientOcclusion, float3 aDiffuseColor, float3 aSpecularColor)
//{
//    const int brdfMipNumber = (int) (aMipCount - MIP_OFFSET);
//    float VdotN = clamp(dot(aToCamera, aViewNormal), 0.0, 1.0);
//    const float3 vRorg = 2.0 * aViewNormal * VdotN - aToCamera;
    
//    float3 vR = GetSpecularDominantDir(aViewNormal, vRorg, RoughnessFromPerceptualRoughness(aPerceptualRoughness));
//    float RdotNsat = saturate(dot(aViewNormal, vR));
    
//    float l = BurleyToMip(aPerceptualRoughness, aMipCount, RdotNsat);
    
//    float3 specularRad = aLysBurleyCube.SampleLevel(defaultSampler, vR, l).xyz;
//    float3 diffuseRad = aLysBurleyCube.SampleLevel(defaultSampler, aViewNormal, (float) (brdfMipNumber - 1)).xyz;
    
//    float t = 1.0 - RdotNsat;
//    float t5 = t * t * t * t * t;
//    aSpecularColor = lerp(aSpecularColor, float3(1.0, 1.0, 1.0), t5);
    
//    float fade = GetReductionInMicroFacets(aPerceptualRoughness);
//    fade *= EmpiricalSpecularAO(anAmbientOcclusion, aPerceptualRoughness);
//    fade *= ApproximateSpecularSelfOcclusion(vR, aNormal);
    
//    float3 ambientDiffuse = anAmbientOcclusion * aDiffuseColor * diffuseRad;
//    float3 ambientSpecular = fade * aSpecularColor * specularRad;
//    return ambientSpecular + ambientSpecular;
//}

//float3 EvaluateDirectionalLight(float3 aDiffuseColor, float3 aSpecularColor, float3 aNormal, float aPerceptualRoughness, float3 aDirectionalLightColor, float3 aLightDirection, float3 aViewDirection)
//{
//    float NdL = saturate(dot(aNormal, aLightDirection));
//    float lambert = NdL;
//    float NdV = saturate(dot(aNormal, aViewDirection));
//    float3 h = normalize(aLightDirection + aViewDirection);
//    float NdH = saturate(dot(aNormal, h));
//    float VdH = saturate(dot(aViewDirection, h));
//    float LdV = saturate(dot(aLightDirection, aViewDirection));
//    float a = max(0.001, aPerceptualRoughness * aPerceptualRoughness);
    
//    float3 diffuseColor = Diffuse(aDiffuseColor);
//    float3 specularColor = Specular(aSpecularColor, h, aViewDirection, a, NdL, NdV, NdH);
    
//    return saturate(aDirectionalLightColor * lambert * (diffuseColor * (1.0 - specularColor) + specularColor) * PI);
//}

//float3 EvaluatePointLight(
//    float3 aDiffuseColor, float3 aSpecularColor, float3 aNormal, float aPerceptualRoughness,
//    float3 aLightColor, float anIntensity, float3 aLightPosition, float aLightRange,
//    float3 aViewDirection, float3 aWorldPosition)
//{
//    float3 lightDirection = aLightPosition - aWorldPosition;
//    float lightDistance = length(lightDirection);
//    lightDirection = normalize(lightDirection);
//    //lightDistance /= 10;
    
//    float NdL = saturate(dot(aNormal, lightDirection));
//    float lambert = NdL;
//    float NdV = saturate(dot(aNormal, aViewDirection));
//    float3 h = normalize(lightDirection + aViewDirection);
//    float NdH = saturate(dot(aNormal, h));
//    float a = max(0.001, aPerceptualRoughness * aPerceptualRoughness);
    
//    float3 diffuseColor = Diffuse(aDiffuseColor);
//    float3 specularColor = Specular(aSpecularColor, h, aViewDirection, a, NdL, NdV, NdH);
    
//    float linearAttenuation = lightDistance / aLightRange;
//    linearAttenuation = 1.0f - linearAttenuation;
//    linearAttenuation = saturate(linearAttenuation);
//    float physicalAttenuation = saturate(1.0f / (lightDistance * lightDistance));
//    float attenuation = lambert * linearAttenuation * physicalAttenuation;
    
//    //anIntensity *= 10000;
    
//    return saturate(aLightColor * anIntensity * attenuation * (diffuseColor * (1.0 - specularColor) + specularColor) * PI);
//}

//float3 EvaluateSpotLight(
//    float3 aDiffuseColor, float3 aSpecularColor, float3 aNormal, float aPerceptualRoughness,
//    float3 aLightColor, float anIntensity, float3 aLightPosition, float aLightRange,
//    float3 aLightDirection, float anOuterAngle, float anInnerAngle,
//    float3 aViewDirection, float3 aWorldPosition)
//{
//    float3 toLight = aLightPosition - aWorldPosition;
//    float lightDistance = length(toLight);
//    toLight = normalize(toLight);
    
//    //if ((lightDistance < 100 && lightDistance > 97.5) || lightDistance < 25)
//    //{
//    //    return (float3)1.0;
//    //}
    
//    float NdL = saturate(dot(aNormal, toLight));
//    float lambert = NdL;
//    float NdV = saturate(dot(aNormal, aViewDirection));
//    float3 h = normalize(toLight + aViewDirection);
//    float NdH = saturate(dot(aNormal, h));
//    float a = max(0.001, aPerceptualRoughness * aPerceptualRoughness);
    
//    float3 diffuseColor = Diffuse(aDiffuseColor);
//    float3 specularColor = Specular(aSpecularColor, h, aViewDirection, a, NdL, NdV, NdH);
    
//    float cosOuterAngle = cos(anOuterAngle);
//    float cosInnerAngle = cos(anInnerAngle);
    
//    // Determine if pixel is within cone.
//    float theta = dot(toLight, normalize(-aLightDirection));
//    // And if we're in the inner or other radius.
//    float epsilon = cosInnerAngle - cosOuterAngle;
//    float intensity = saturate((theta - cosOuterAngle) / epsilon);
//    intensity *= intensity;
    
//    float linearAttenuation = lightDistance / aLightRange;
//    linearAttenuation = 1.0f - linearAttenuation;
//    linearAttenuation = saturate(linearAttenuation);
//    float physicalAttenuation = saturate(1.0f / (lightDistance * lightDistance));
//    float attenuation = lambert * linearAttenuation * physicalAttenuation;
    
//    float finalAttenuation = lambert * intensity * attenuation;
    
//    //anIntensity *= 10000;
    
//    return saturate(aLightColor * anIntensity * lambert * finalAttenuation * (diffuseColor * (1.0 - specularColor) + specularColor) * PI);
//}