#pragma once
#include <memory>
#include <vector>
#include <CommonUtilities\MathBundle.hpp>
class AudioComponent;
class Input;

class PlayerScript;
class EnemyScript;
class PlayerSystem;

class UIManager;
namespace SE
{
    class CContentLoader;
    class CModelInstance;
    class CSprite;
    struct SNavMesh;
}

class GameWorld;
namespace Game
{
    class Game
    {
    public:
        ~Game();
        bool Init();
        bool Update();

        void DisplayImGui();

    private:
        void InitGameScene();
        void InitMainmenuScene();

    private:
        GameWorld* myGameWorld;

        bool myIsWalking = false;
        //Input* myInput;


        float myTime = 0;
        float myCameraOffsetZ = -280.0f;
        float myCameraOffsetY = 1000.0f;
        float myCameraRotation = 70.0f;

        float3 myLightDir = { 0, 0, 0 };

        std::vector<SE::CSprite*> mySprites;
    };
}