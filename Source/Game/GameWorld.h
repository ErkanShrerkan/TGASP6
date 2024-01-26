#pragma once
#include "LevelHandler.h"
#include "AudioSystem.h"
#include "ParticleEditor.h"
namespace SE
{
    class CSprite;
    class CCamera;
}
class GameWorld
{
public:
    void Init(SE::CCamera* aCamera);
    void Update();

    constexpr static GameWorld*& GetInstance() noexcept { return ourInstance; }
    // TODO: use postmaster instead to change level
    inline LevelHandler& GetLevelHandler()
    {
        return myLevelHandler;
    }
    inline SE::CCamera* GetCamera() { return myCamera; }
private:
    static GameWorld* ourInstance;

    SE::CSprite* myVignette;
    SE::CCamera* myCamera;
    LevelHandler myLevelHandler;
    ParticleEditor myParticleEditor;
};
