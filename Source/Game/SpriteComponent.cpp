#include "pch.h"
#include "SpriteComponent.h"

SpriteComponent::SpriteComponent(SE::CGameObject& aParent)
    : SE::CComponent(aParent)
{
    mySprite = nullptr;
}

SpriteComponent::~SpriteComponent()
{
}

void SpriteComponent::Init(const char* myPath)
{
    (myPath);
}

void SpriteComponent::Update()
{
}