#include "pch.h"
#include "GameWorld.h"
#include <fstream>
#include <Engine\Camera.h>
#include <Engine\Sprite.h>
GameWorld* GameWorld::ourInstance = nullptr;

#include <Engine\DX11.h>
#include <Engine\ContentLoader.h>

#include <pix3.h>


GameWorld::~GameWorld()
{
    myVignette->Release();
}

void GameWorld::Init(SE::CCamera* aCamera)
{
    myCamera = aCamera;
    myLevelHandler.Init("Data/Levels.json");
    ourInstance = this;

    myVignette = SE::DX11::Content->GetSpriteFactory().GetSprite("Textures/Vignette.dds");

    myVignette->SetSizeRelativeToScreen({ 1.0f, 1.0f });

    myParticleEditor.Init("Data/Particles.json");
}

void GameWorld::Update()
{
#ifdef DEBUG
    PIXBeginEvent(PIX_COLOR_INDEX(1), __FUNCTION__);
#endif // DEBUG

    myLevelHandler.UpdateActiveLevel();
    //myVignette->Render();

#ifdef DEBUG
    myParticleEditor.OnImGui();
    PIXEndEvent();
#endif // DEBUG
}

