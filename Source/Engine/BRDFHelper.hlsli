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