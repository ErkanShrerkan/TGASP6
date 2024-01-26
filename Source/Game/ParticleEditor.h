#pragma once
#include <string>
#include <vector>
#include <Engine/ParticleEmitter.h>
class JsonDocument;
class ParticleEditor
{
private:
    JsonDocument* myDocument;

public:
    void Init(const std::string& aConfigFile);
    void OnImGui();

private:
    char myCharBuffer[128];
    char myPathBuffer[128];
    void UpdateCharBuffer(char(*aBuffer)[128], const std::string& aString);

    void WriteParticleSettings(const std::string& aName, SE::CParticleEmitter::SParticleSettings& someSettings);

private:
    std::string myConfigFile;
    struct ParticlePreset
    {
        std::string name;
        std::string sprite;
        std::string oldSprite;
        SE::CParticleEmitter::SParticleSettings settings;
    };
    size_t mySelectedPresetIndex = 0;
    std::vector<ParticlePreset> myParticlePresets;
};
