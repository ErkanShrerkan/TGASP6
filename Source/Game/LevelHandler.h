#pragma once
#include "Level.h"
#include <vector>
#include <string>
#include "Experience.h"
#include <Engine\Sprite.h>
#include <Engine\Engine.h>

class LevelHandler
{
private:
    struct LevelSettings
    {
        float waterLevel;
        float3 shallowWaterColor;
        float3 deepWaterColor;
        float3 waterBorderColor;
        float waterBorderThreshold;
        float depthMult;
        float alphaMult;

        float3 fogColor;
        float fogMultiplier;
        float fogDensity;
        float fogOffset;

        float godRaySampleDistance;
        float godRayFalloff;
        float alphaThreshold;

        float4 lightColor;
        float4 ambientColor;

        float3 lightDirection;
        float2 cameraRotation;
        float2 cameraOffset;
        float cameraDistance;
    };

private:
    std::vector<std::string> myLevelPaths;
    std::vector<LevelSettings> myLevelSettings;
    int myActiveLevelIndex = 0;
    Level* myLevel = nullptr;
    bool myLoadLevelNextFrame = false;
    bool myLoadLevelFromLevelSelect = false;
    Experience myExperience;

    SE::CSprite* myLoadingScreen = nullptr;

private:
    void LoadLevel(int anIndex);

public:
    ~LevelHandler();
    void Init(const std::string& aLevelConfig);
    void UpdateActiveLevel();
    inline Level& GetActiveLevel()
    {
        return *myLevel;
    }
    inline void SetLevelByIndex(int anIndex, const bool& aFromLevelSelect)
    {
        myActiveLevelIndex = anIndex;
        myLoadLevelNextFrame = true;
        myLoadLevelFromLevelSelect = aFromLevelSelect;
        myLoadingScreen->Render();
        SE::CEngine::GetInstance()->GetActiveScene()->SetLoadingFrame(true);
    }
    inline int GetActiveLevelIndex() const
    {
        return myActiveLevelIndex;
    }
    inline LevelSettings& GetActiveLevelSettings()
    {
        return myLevelSettings[myActiveLevelIndex];
    }
    inline LevelSettings& GetLevelSettingsByIndex(int aIndex)
    {
        return myLevelSettings[aIndex];
    }
};
